#include <iostream>

using std::cout;
using std::endl;

#include "rabbitmq.h"
#include "common.h"

#if defined(_DEBUG) || defined(DEBUG)
# pragma comment(lib,"librabbitmqd.4.lib")
#else
# pragma comment(lib,"librabbitmq.4.lib")
#endif


int main()
{
	RabbitMQ rmq;
	rmq.connect("127.0.0.1",5672,5,1);
	rmq.declarExchange("test1", RabbitMQ::DIRECT);
	rmq.declareQueue("test1.1");
	rmq.bind("test1", "test1.1", "test");
	int countp = 10000;
	while (countp--)
	{
		rmq.publish("test1", "test", "Hello World", strlen("Hello World"));
	}
	rmq.consume("test1.1");
	system("pause");
	return 0;
}