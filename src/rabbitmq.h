#ifndef _RABBITMQ_H_
#define _RABBITMQ_H_

#include <iostream>
#include <vector>

#include <amqp.h>
#include <amqp_tcp_socket.h>

class Rabbitmq {

public:

	Rabbitmq(const char*host, short port);
	~Rabbitmq();
	void conn(int timeout);
	void declare_exchange(const char* exchange, const char* exchangetype);
	void send(const char*exchange, const char* routeKey, const char* data);
	void bind(const char*exchange, const char* bindkey);
	void unbind(const char*exchange, const char* bindkey);

private:
	/* conn info */
	std::string					m_host;
	short						m_port;
	std::vector<std::string>	m_vexchange;
	/* rabbitmq handlers */
	amqp_socket_t*				m_socket;
	amqp_connection_state_t		m_conn;
};

#endif //_RABBITMQ_H_