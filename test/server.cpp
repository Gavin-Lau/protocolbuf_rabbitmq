#include <iostream>
#include <cstdlib>
#include <windows.h>

#include "rabbitmq.h"
#include "microtimer.h"
#include "pack.h"

#ifdef _MSC_VER
# if  defined(_DEBUG) || defined(DEBUG)
#  pragma comment(lib, "libprotobufd.lib")
#  pragma comment(lib, "librabbitmqd.4.lib")
# else
#  pragma comment(lib, "libprotobuf.lib")
#  pragma comment(lib, "librabbitmq.4.lib")
# endif
#endif

using std::cout;
using std::endl;

//simple echo server
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
		unpack(message);
		//cout << "server message reveived : " << message.c_str() << endl;
		std::string replyque = rmq.getReplyQ();
		rmq.publish("test1", replyque.c_str(), message.c_str(), message.length());
	}
	cout << "Server received and echo[10000]times, time espired[" << timer.expired() << "]ms." << endl;
}
