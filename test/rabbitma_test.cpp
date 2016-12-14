#include <iostream>

using std::cout;
using std::endl;

#include "common.h"
#include "timer.h"


#include "rabbitmq.h"

int main()
{
	RabbitMQ rmq;
	rmq.connect("127.0.0.1",5672,5,1);
	rmq.declarExchange("test1", RabbitMQ::DIRECT);
	rmq.declareQ("test1.1");
	rmq.bind("test1", "test1.1", "test");
	int countp = 10005;
	MicroTimer timer;

	// rpc single mode
	timer.reset();
	rmq.declareQ("rpc"); //declare rpc Q
	rmq.bind("test1", "rpc", "rpc");
	rmq.publishRPC("test1", "rpc", "test", "Hello World", strlen("Hello World"));
	std::string message = "echo " + *rmq.get("test1.1");
	rmq.publish("test1", "rpc", message.c_str(), message.length());
	std::string messageReplied = *rmq.get("rpc");

	// single get
	timer.reset();
	rmq.publish("test1", "test", "Hello World", strlen("Hello World"));
	cout << rmq.get("test1.1")->c_str() << endl;
	cout << "send/receive \"Hello World\"[1]times, time expired:" << timer.expired() << "ms" << endl;

#if 0
	// forget after publish  banches 
	while (countp--)
	{
		rmq.publish("test1", "test", "Hello World", strlen("Hello World"));
	}
	cout << "send \"Hello World\"[10000]times, time expired:" << timer.expired() << "ms" << endl;
	timer.reset();
	rmq.consumeBegin("test1.1");
	countp = 10000;
	while (countp--)
	{
		rmq.consume();
	}
	rmq.consumeEnd();
	cout << "receive\"Hello World\"[10000]times, time expired:" << timer.expired() << "ms" << endl;

#endif
	system("pause");
	return 0;
}
