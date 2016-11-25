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

void RabbitMQ::connect(const char*host, unsigned short port, double timeout, int channel = DEFAULT_CHANNEL)
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
			amqp_cstring_bytes(ExTypeName[exchangeType].c_str()),
			0, 0, 0, 0, amqp_empty_table);
	amqp_rpc_reply_t ret = amqp_get_rpc_reply(conn);
	if (ret.reply_type != AMQP_RESPONSE_NORMAL)
	{
		errnum = RBT_DECLERE_EXCHANGE_FAIL;
	}
}

void RabbitMQ::declareQueue(const char* queue, EXCHANGE_TYPE exchangeType)
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

void RabbitMQ::send(const char*exchange, const char* routeKey, const char* data, size_t len)
{
	if (errnum < 0) return;
	amqp_bytes_t sbuf;
	sbuf.bytes = (void*)data;
	sbuf.len = len;

	amqp_basic_properties_t props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 2; /* persistent delivery mode */

	amqp_basic_publish(conn, DEFAULT_CHANNEL,
		amqp_cstring_bytes(exchange),
		amqp_cstring_bytes(routeKey),
		0, 0, &props, sbuf);
	
}

int RabbitMQ::getRabbitmqErrno(int ret)
{

}

std::string RabbitMQ::getRabbitmqErrstr(int err)
{

}

