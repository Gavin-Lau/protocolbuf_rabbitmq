#include <iostream>
#include <cstdlib>
#include <windows.h>

#include "rabbitmq.h"
#include "timer.h"

using std::cout;
using std::endl;

int main()
{
	RabbitMQ rmq(RabbitMQ::SERVER);
	rmq.connect("127.0.0.1",5672,5,1);
	rmq.declarExchange("test1", RabbitMQ::DIRECT);
	rmq.declareQ("test1.1");
	rmq.declareQ("rpc"); //declare rpc Q
	rmq.bind("test1", "test1.1", "test");
	rmq.consumeBegin("test1.1");
	int count = 10000;
	MicroTimer timer;
	while (count--)
	{
		std::string message = *rmq.consume();
		//cout << "server message reveived : " << message.c_str() << endl;
		std::string replyque = rmq.getReplyQ();
		rmq.publish("test1", replyque.c_str(), message.c_str(), message.length());
	}
	cout << "接收并返回\"Hello World\"[10000]次，用时[" << timer.expired() << "]ms." << endl;
}
