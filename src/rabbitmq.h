#ifndef _RABBITMQ_H_
#define _RABBITMQ_H_


#include <iostream>
#include <vector>

#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <amqp_framing.h>

#include "common.h"
#define		DEFAULT_CHANNEL		1

class RabbitMQ{

public:
	enum EXCHANGE_TYPE { DIRECT = 0, FANOUT, TOPIC };
	enum TYPE { SERVER = 0, CLIENT};
	RabbitMQ(TYPE typ);
	~RabbitMQ();
	void connect(const char*host, unsigned short port, double timeout, int);
	void declarExchange(const char* exchange, EXCHANGE_TYPE exchangeType);
	void declareQ(const char* queue);
	std::string getReplyQ() { return replyToQ; }
	/** must be clared first */
	void bind(const char*exchange, const char* queue, const char* bindkey);
	void unbind(const char*exchange, const char* queue, const char* bindkey);

	void publish(const char*exchange, const char* routeKey, const char* data, size_t len);
	void publishRPC(const char*exchange, const char* replyQ, const char* routeKey, const char* data, size_t len);

	/** subscribe -> get next msg(only one message) -> unsubscribe
		if the message is RPC type,replyQ will be set
	*/
	std::string* get(const char* queue);
	/** high performance for bulk/large-amount data*/
	void consumeBegin(const char*queue);

	/** if the message is RPC type,replyQ will be set */
	std::string* consume();
	void consumeEnd()  { }

	std::string getRabbitmqErrstr(amqp_rpc_reply_t& ret);

private:

	static char ExTypeName[3][15]; //msvc do not allow initial here, Micro. S**K!
	/* conn info */
	std::vector<std::string>	exchanges;
	/* rabbitmq handlers */
	amqp_socket_t*				socket;
	amqp_connection_state_t		conn;
	int							errnum;
	std::string					errmsg;
	TYPE						type;

	std::string*				recbuf;
	std::string					replyToQ;

};

#endif //_RABBITMQ_H_
