#include <iostream>
#include <windows.h>

#include "rabbitmq.h"
#include "microtimer.h"
#include "pack.h"

using std::cout;
using std::endl;

#ifdef _MSC_VER
# if  defined(_DEBUG) || defined(DEBUG)
#  pragma comment(lib, "libprotobufd.lib")
#  pragma comment(lib, "librabbitmqd.4.lib")
# else
#  pragma comment(lib, "libprotobuf.lib")
#  pragma comment(lib, "librabbitmq.4.lib")
# endif
#endif

int main()
{
	std::string sendbuf;
	pack(sendbuf);
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
		rmq.publishRPC("test1", "rpc", "test", sendbuf.c_str(), sendbuf.length());
		rmq.consumeBegin("rpc");

		std::string message = *rmq.consume();
		unpack(message);
		//cout << "client message reveived : " << message.c_str() << endl;
		std::string replyque = rmq.getReplyQ();
		rmq.publish("test1", replyque.c_str(), message.c_str(), message.length());
	}
	cout << "Client send and received[10000]times,time expired[" << timer.expired() << "]ms." << endl;
	system("pause");
	return 0;
}
