#include <iostream>
#ifdef _MSC_VER
# include <winsock2.h>
# pragma comment(lib,"ws2_32.lib")
#else
# include <ctime>
#endif


#include "rabbitmq.h"

//#define MYDEBUG

using std::cout;
using std::endl;

const char RabbitMQ::ExTypeName[3][15] = { "direct", "fanout", "topic" };

RabbitMQ::RabbitMQ()
	: recbuf(new std::string)
{
	conn = amqp_new_connection();
	socket = amqp_tcp_socket_new(conn);
	if (!socket)
	{
		errnum = RBT_INIT_FAIL;
	} else {
		errnum = OK;
	}
}

RabbitMQ::~RabbitMQ()
{
	amqp_channel_close(conn, DEFAULT_CHANNEL, AMQP_REPLY_SUCCESS);
	amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
	amqp_destroy_connection(conn);
	delete recbuf;
}

void RabbitMQ::connect(const char*host = "127.0.0.1",
			unsigned short port = 5672,
			double timeout = 5.0, int channel = DEFAULT_CHANNEL)
{
	if (errnum < 0) return;
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = (timeout - (int)timeout) * 1000000;

	if (amqp_socket_open_noblock(socket, host, port, &tv) < 0)
	{
		errnum = RBT_CONN_TIMEOUT;
		return;
	}
	amqp_rpc_reply_t ret = amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_LOGIN_FAIL;
		return;
	}
	amqp_channel_open(conn, channel);
	ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_CHANNEL_OPEN_FAIL;
		return;
	}
}

void RabbitMQ::declarExchange(const char* exchange, EXCHANGE_TYPE exchangeType)
{
	if (errnum < 0) return;
	/*amqp_exchange_declare(amqp_connection_state_t state, amqp_channel_t channel,
					amqp_bytes_t exchange, amqp_bytes_t type,
					amqp_boolean_t passive, amqp_boolean_t durable,
					amqp_boolean_t auto_delete, amqp_boolean_t internal, amqp_table_t arguments);
	*/
	amqp_exchange_declare(conn, DEFAULT_CHANNEL,
			amqp_cstring_bytes(exchange),
			amqp_cstring_bytes(ExTypeName[exchangeType]),
			0, //仅仅想查询某一个队列是否已存在，如果不存在，不想建立该队列，仍然可以调用queue.declare，只不过需要将参数passive设为true
			0, //队列和交换机有一个创建时候指定的标志durable，直译叫做坚固的。durable的唯一含义就是具有这个标志的队列和交换机会在重启之后重新建立，它不表示说在队列当中的消息会在重启后恢复。
			0, //当所有绑定队列都不再使用时，是否自动删除该交换机。
			0, //表示这个exchange不可以被client用来推送消息，仅用来进行exchange和exchange之间的绑定。
			amqp_empty_table);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_DECLERE_EXCHANGE_FAIL;
	}
}

void RabbitMQ::declareQ(const char* queue)
{
	/*amqp_queue_declare(amqp_connection_state_t state, amqp_channel_t channel,
				amqp_bytes_t queue, amqp_boolean_t passive, amqp_boolean_t durable,
				amqp_boolean_t exclusive, amqp_boolean_t auto_delete,
				amqp_table_t arguments);
	*/
	amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, DEFAULT_CHANNEL,
			amqp_cstring_bytes(queue),
			0, //当参数为true时，只是用来查询队列是否存在，不存在情况下不会新建队列
			0, //队列是否持久化
			0, //当前连接不在时，队列是否自动删除
			0, //没有consumer时，队列是否自动删除
			amqp_empty_table);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_DECLERE_QUEUE_FAIL;
	}
}

void RabbitMQ::bind(const char*exchange, const char* queue, const char* bindkey)
{
	amqp_queue_bind(conn, DEFAULT_CHANNEL,
		amqp_cstring_bytes(queue),
		amqp_cstring_bytes(exchange),
		amqp_cstring_bytes(bindkey),
		amqp_empty_table);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_BIND_FAIL;
	}
}

void RabbitMQ::unbind(const char*exchange, const char* queue, const char* bindkey)
{
	amqp_queue_unbind(conn, DEFAULT_CHANNEL,
		amqp_cstring_bytes(queue),
		amqp_cstring_bytes(exchange),
		amqp_cstring_bytes(bindkey),
		amqp_empty_table);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_BIND_FAIL;
	}
}

void RabbitMQ::publish(const char*exchange, const char* routeKey, const char* data, size_t len)
{
	if (errnum < 0) return;
	amqp_bytes_t sbuf;
	sbuf.bytes = (void*)data;
	sbuf.len = len;

	amqp_basic_properties_t props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 2;//non-persistent (1) or persistent (2)

	/**
	* AMQP_CALL amqp_basic_publish(amqp_connection_state_t state, amqp_channel_t channel,
							amqp_bytes_t exchange, amqp_bytes_t routing_key,
							amqp_boolean_t mandatory, amqp_boolean_t immediate,
							struct amqp_basic_properties_t_ const *properties,
							amqp_bytes_t body);
	*/
	amqp_basic_publish(conn, DEFAULT_CHANNEL,
		amqp_cstring_bytes(exchange),
		amqp_cstring_bytes(routeKey),
		0, 0, &props, sbuf);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_PUBLISH_FAIL;
	}
}

void RabbitMQ::consumeBegin(const char*queue)
{
	/**
	amqp_basic_consume(amqp_connection_state_t state, amqp_channel_t channel,
				amqp_bytes_t queue, amqp_bytes_t consumer_tag,
				amqp_boolean_t no_local, amqp_boolean_t no_ack,
				amqp_boolean_t exclusive, amqp_table_t arguments);
	*/
	amqp_basic_consume(conn, DEFAULT_CHANNEL,
		amqp_cstring_bytes(queue), amqp_empty_bytes,
		0, 1, 0, amqp_empty_table);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_CONSUME_FAIL;
	}
}

///** consumer type1: as subscribe  tested:faster */
//std::string* RabbitMQ::consume()
//{
//	recbuf->clear();
//	amqp_maybe_release_buffers(conn);
//	amqp_rpc_reply_t ret = amqp_consume_message(conn, &envelope, NULL, 0);
//	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
//	{
//		errnum = RBT_CONSUME_FAIL;
//		return NULL;
//	}
//	const amqp_basic_properties_t& props = envelope.message.properties;
//	recbuf->append( std::string((char*)envelope.message.body.bytes, envelope.message.body.len));
//
//#ifdef MYDEBUG
//	printf("Delivery %u, exchange %.*s routingkey %.*s\n",
//		(unsigned)envelope.delivery_tag,
//		(int)envelope.exchange.len, (char *)envelope.exchange.bytes,
//		(int)envelope.routing_key.len, (char *)envelope.routing_key.bytes);
//
//	if (envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
//		printf("Content-type: %.*s\n",
//			(int)envelope.message.properties.content_type.len,
//			(char *)envelope.message.properties.content_type.bytes);
//	}
//	if (props.reply_to.bytes != NULL)
//	{
//		printf("Reply to queue: %.*s\n", props.reply_to.bytes, props.reply_to.len);
//	}
//	printf("Message body:%s\n", recbuf->c_str());
//#endif
//
//	return recbuf;
//}

/** consumer type2: using frame api  tested:slower than type1 */
std::string* RabbitMQ::consume()
{
	amqp_frame_t frame;
	amqp_basic_deliver_t *d;
	amqp_basic_properties_t *p;
	size_t body_target;
	size_t body_received;
	int ret;

	amqp_maybe_release_buffers(conn);
	ret = amqp_simple_wait_frame(conn, &frame);
	if (ret != AMQP_STATUS_OK)  return NULL;
	if (frame.frame_type != AMQP_FRAME_METHOD) return NULL;
	if (frame.payload.method.id != AMQP_BASIC_DELIVER_METHOD) return NULL;
	d = (amqp_basic_deliver_t *)frame.payload.method.decoded;
#ifdef MYDEBUG
	printf("Frame type: %u channel: %u\n", frame.frame_type,
		frame.channel);
	printf("Method: %s\n", amqp_method_name(frame.payload.method.id));

	printf("Delivery: %u exchange: %.*s routingkey: %.*s\n",
		(unsigned)d->delivery_tag,
		(int)d->exchange.len, (char *)d->exchange.bytes,
		(int)d->routing_key.len, (char *)d->routing_key.bytes);
#endif
	ret = amqp_simple_wait_frame(conn, &frame);
	if (ret < 0) return NULL;
	if (frame.frame_type != AMQP_FRAME_HEADER) return NULL;
	p = (amqp_basic_properties_t *)frame.payload.properties.decoded;
	if (p->_flags & AMQP_BASIC_CONTENT_TYPE_FLAG == 0) return NULL;

#ifdef MYDEBUG
	printf("Content-type: %.*s\n",
			(int)p->content_type.len, (char *)p->content_type.bytes);
#endif
	//receiving until message end
	body_target = (size_t)frame.payload.properties.body_size;
	body_received = 0;

	while (body_received < body_target) {
		ret = amqp_simple_wait_frame(conn, &frame);
		if (ret < 0) break;

		if (frame.frame_type != AMQP_FRAME_BODY) return NULL;

		body_received += frame.payload.body_fragment.len;
		std::string message_((char*)frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);
	}
}

std::string* RabbitMQ::get(const char* queue)
{
	amqp_rpc_reply_t ret
		= amqp_basic_get(conn, DEFAULT_CHANNEL, amqp_cstring_bytes(queue), 1);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_GET_FAIL;
		return NULL;
	}

	if (ret.reply.id == AMQP_BASIC_GET_EMPTY_METHOD)
	{
		errnum = RBT_GET_NOTHING;
		return NULL;
	}
	
	size_t body_remaining;
	amqp_frame_t frame;
	if (AMQP_STATUS_OK != amqp_simple_wait_frame(conn, &frame))
	{
		errnum = RBT_GET_CONTENT_FAIL;
		return NULL;
	}

	if (frame.frame_type != AMQP_FRAME_HEADER)
	{
		errnum = RBT_WRONG_FRAME;
		return NULL;
	}

	body_remaining = frame.payload.properties.body_size;
	while (body_remaining) {
		if (AMQP_FRAME_BODY != amqp_simple_wait_frame(conn, &frame))
		{
			errnum = RBT_GET_CONTENT_FAIL;
			return NULL;
		}
		if (AMQP_FRAME_BODY != frame.frame_type)
		{
			errnum = RBT_WRONG_FRAME;
			return NULL;
		}
		body_remaining -= frame.payload.body_fragment.len;
		recbuf->append(frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);
	}

}

int RabbitMQ::getRabbitmqErrno(int ret)
{
	return 0;
}

std::string RabbitMQ::getRabbitmqErrstr(int err)
{
	return "";
}

