#include <iostream>

using std::cout;
using std::endl;

#include "rabbitmq.h"
#include "common.h"
#include "timer.h"

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
	rmq.declareQ("test1.1");	
	rmq.bind("test1", "test1.1", "test");
	int countp = 10000;

	// ������ģʽ
	MicroTimer timer;
	while (countp--)
	{
		rmq.publish("test1", "test", "Hello World", strlen("Hello World"));
	}
	cout << "����\"Hello World\"[10000]��,��ʱ��" << timer.expired() << "ms" << endl;
	timer.reset();
	rmq.consumeBegin("test1.1");
	countp = 10000;
	while (countp--)
	{
		rmq.consume();
	}
	rmq.consumeEnd();
	cout << "����\"Hello World\"[10000]��,��ʱ��" << timer.expired() << "ms" << endl;
	
	// rpcģʽ

	system("pause");
	return 0;
}