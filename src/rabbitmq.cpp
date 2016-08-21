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

Rabbitmq::Rabbitmq(const char*host, short port)
	:
	m_host(host),
	m_port(port)
{
}

Rabbitmq::~Rabbitmq()
{
	amqp_channel_close(m_conn, 1, AMQP_REPLY_SUCCESS);
	amqp_connection_close(m_conn, AMQP_REPLY_SUCCESS);
	amqp_destroy_connection(m_conn);
}


void Rabbitmq::conn(int timeout)
{
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	m_conn = amqp_new_connection();
	m_socket = amqp_tcp_socket_new(m_conn);

	amqp_socket_open_noblock(m_socket, m_host.c_str(), m_port, &tv);
	amqp_login(m_conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "guest", "guest");
}

void Rabbitmq::declare_exchange(const char* exchange, const char* exchangetype)
{
	amqp_exchange_declare(m_conn, 1, amqp_cstring_bytes(exchange), amqp_cstring_bytes(exchangetype),
		0, 0, 0, 0, amqp_empty_table);
	amqp_get_rpc_reply(m_conn);
}

void Rabbitmq::send(const char*exchange, const char* routeKey, const char* data)
{
	amqp_basic_properties_t props;
	props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	props.content_type = amqp_cstring_bytes("text/plain");
	props.delivery_mode = 2; /* persistent delivery mode */
	amqp_basic_publish(m_conn,
		1,
		amqp_cstring_bytes(exchange),
		amqp_cstring_bytes(routeKey),
		0,
		0,
		&props,
		amqp_cstring_bytes(data)	);
}

void Rabbitmq::bind(const char*exchange, const char* bindkey)
{

}

void Rabbitmq::unbind(const char*exchange, const char* bindkey)
{

}
