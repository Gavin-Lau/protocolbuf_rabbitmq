#include <iostream>
#include <windows.h>

#include "rabbitmq.h"
#include "timer.h"

using std::cout;
using std::endl;

int main()
{
	RabbitMQ rmq(RabbitMQ::CLIENT);
	rmq.connect("127.0.0.1",5672,5,1);
	rmq.declarExchange("test1", RabbitMQ::DIRECT);
	rmq.declareQ("test1.1");
	rmq.declareQ("rpc"); //declare rpc Q
	rmq.bind("test1", "rpc", "rpc");
	int count = 10000;
	MicroTimer timer;
	while (count--)
	{
		rmq.publishRPC("test1", "rpc", "test", "Hello World", strlen("Hello World"));
		rmq.consumeBegin("rpc");

		std::string message = *rmq.consume();
		//cout << "client message reveived : " << message.c_str() << endl;
		std::string replyque = rmq.getReplyQ();
		rmq.publish("test1", replyque.c_str(), message.c_str(), message.length());
	}
	cout << "发送并接收\"Hello World\"[10000]次，用时[" << timer.expired() << "]mszs." << endl;
    system("pause");
    return 0;
}
