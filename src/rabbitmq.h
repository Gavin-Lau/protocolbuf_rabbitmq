#ifndef _RABBITMQ_H_
#define _RABBITMQ_H_

#include <iostream>
#include <vector>

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_framing.h>

#include "common.h"
#define		DEFAULT_CHANNEL		1
#define		DEFAULT_AUTODEL		0
class RabbitMQ{

public:
	enum EXCHANGE_TYPE {DIRECT = 0, FANOUT, TOPIC};
	RabbitMQ();
	~RabbitMQ();
	void connect(const char*host, unsigned short port, double timeout, int);
	void declarExchange(const char* exchange, EXCHANGE_TYPE exchangeType);
	void declareQ(const char* queue);
	/** must be clared first */
	void setReplyQ(const char* queue) { replyToQ = queue; }
	void bind(const char*exchange, const char* queue, const char* bindkey);
	void unbind(const char*exchange, const char* queue, const char* bindkey);

	void publish(const char*exchange, const char* routeKey, const char* data, size_t len);

	/** subscribe -> get next msg(only one message) -> unsubscribe TODO*/
	void get();

	/** high performance for bulk/large-amount data*/
	void consumeBegin(const char*queue);
	std::string* consume();
	std::string* consumeRPC(const char* replyMsg);
	void consumeEnd()  { amqp_destroy_envelope(&envelope); }

	int getRabbitmqErrno(int ret);
	std::string getRabbitmqErrstr(int err);

private:

	static const char ExTypeName[3][15]; //msvc do not allow initial here, Micro. S**K!
	/* conn info */
	std::vector<std::string>	exchanges;
	/* rabbitmq handlers */
	amqp_socket_t*				socket;
	amqp_connection_state_t		conn;
	int							errnum;

	amqp_envelope_t				envelope;
	std::string*				recbuf;
	std::string					replyToQ;

};

#endif //_RABBITMQ_H_