#include <iostream>

using std::cout;
using std::endl;

#include "inipaser.h"
#include "rabbitmq.h"
#include "common.h"

#if defined(_DEBUG) || defined(DEBUG)
# pragma comment(lib,"librabbitmqd.4.lib")
#else
# pragma comment(lib,"librabbitmq.4.lib")
#endif


int main()
{
	IniParser inipaser;
	inipaser.parser("../conf.ini");
	
	Rabbitmq rmq("127.0.0.1", 5672);
	system("pause");
	return 0;
}