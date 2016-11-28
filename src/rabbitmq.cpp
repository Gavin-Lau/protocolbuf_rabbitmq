#include <iostream>
#ifdef _MSC_VER
# include <winsock2.h>
# pragma comment(lib,"ws2_32.lib")
#else
# include <ctime>
#endif


#include "rabbitmq.h"

using std::cout;
using std::endl;

const char RabbitMQ::ExTypeName[3][15] = { "direct", "fanout", "topic" };

RabbitMQ::RabbitMQ()
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
	//amqp_exchange_declare(amqp_connection_state_t state, amqp_channel_t channel, amqp_bytes_t exchange, amqp_bytes_t type, amqp_boolean_t passive, amqp_boolean_t durable, amqp_boolean_t auto_delete, amqp_boolean_t internal, amqp_table_t arguments);
	amqp_exchange_declare(conn, DEFAULT_CHANNEL, 
			amqp_cstring_bytes(exchange),
			amqp_cstring_bytes(ExTypeName[exchangeType]),
			0, 0, 0, 0, amqp_empty_table);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_DECLERE_EXCHANGE_FAIL;
	}
}

void RabbitMQ::declareQueue(const char* queue)
{
	//amqp_queue_declare(amqp_connection_state_t state, amqp_channel_t channel, amqp_bytes_t queue, amqp_boolean_t passive, amqp_boolean_t durable, amqp_boolean_t exclusive, amqp_boolean_t auto_delete, amqp_table_t arguments);
	amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, DEFAULT_CHANNEL, 
			amqp_cstring_bytes(queue), 0, 0, 0, 1,
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

int RabbitMQ::getRabbitmqErrno(int ret)
{

}

void RabbitMQ::consume(const char* queue)
std::string RabbitMQ::getRabbitmqErrstr(int err)
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

	amqp_envelope_t envelope;
	amqp_maybe_release_buffers(conn);
	ret = amqp_consume_message(conn, &envelope, NULL, 0);

	printf("Delivery %u, exchange %.*s routingkey %.*s\n",
		(unsigned)envelope.delivery_tag,
		(int)envelope.exchange.len, (char *)envelope.exchange.bytes,
		(int)envelope.routing_key.len, (char *)envelope.routing_key.bytes);

	if (envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
		printf("Content-type: %.*s\n",
			(int)envelope.message.properties.content_type.len,
			(char *)envelope.message.properties.content_type.bytes);
	}

	std::string message_ = std::string((char*)envelope.message.body.bytes, envelope.message.body.len);

	amqp_destroy_envelope(&envelope);
}

void RabbitMQ::get()
{
	return 0;
}

void RabbitMQ::consume(const char* queue)
{
	return "";

	amqp_basic_consume(conn, 1, amqp_cstring_bytes(queue), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_CONSUME_FAIL;
	}

	amqp_maybe_release_buffers(conn);
	ret = amqp_consume_message(conn, &envelope, NULL, 0);
}

