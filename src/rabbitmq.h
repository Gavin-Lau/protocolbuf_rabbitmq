#ifndef _RABBITMQ_H_
#define _RABBITMQ_H_

#include <iostream>
#include <vector>

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include "common.h"
#define		DEFAULT_CHANNEL 1
class RabbitMQ{

public:
	enum EXCHANGE_TYPE {DIRECT = 0, FANOUT, TOPIC};
	RabbitMQ();
	~RabbitMQ();
	void connect(const char*host, unsigned short port, double timeout, int);
	void declarExchange(const char* exchange, EXCHANGE_TYPE exchangeType);
	void declareQueue(const char* exchange, EXCHANGE_TYPE exchangeType);
	void send(const char*exchange, const char* routeKey, const char* data, size_t len);
	void bind(const char*exchange, const char* queue, const char* bindkey);
	void unbind(const char*exchange, const char* queue, const char* bindkey);

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