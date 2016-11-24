#ifndef _RABBITMQ_H_
#define _RABBITMQ_H_

#include <iostream>
#include <vector>

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include "common.h"

class RabbitMQ{

public:
	enum EXCHANGE_TYPE {DIRECT = 0, FANOUT, TOPIC};
	RabbitMQ();
	~RabbitMQ();
	void connect(const char*host, unsigned short port, double timeout, int channel = 1);
	void declarExchange(const char* exchange, EXCHANGE_TYPE exchangeType);
	void send(const char*exchange, const char* routeKey, const char* data);
	void bind(const char*exchange, const char* bindkey);
	void unbind(const char*exchange, const char* bindkey);
	int getRabbitmqErrno(int ret);
	std::string getRabbitmqErrstr(int err);
private:
	const std::string ExTypeName[3] = {"direct", "fanout", "topic"};
	/* conn info */
	std::vector<std::string>	exchanges;
	/* rabbitmq handlers */
	amqp_socket_t*				socket;
	amqp_connection_state_t		conn;
	int							errnum;
};

#endif //_RABBITMQ_H_