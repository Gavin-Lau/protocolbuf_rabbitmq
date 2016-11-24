#include <iostream>

using std::cout;
using std::endl;

#include "inipaser.h"
#include "rabbitmq.h"
#include "common.h"

#pragma comment(lib,"rabbitmq.4.lib")


int main()
{
	IniParser inipaser;
	inipaser.parser("../conf.ini");
	
	Rabbitmq rmq("127.0.0.1", 5672);
	system("pause");
	return 0;
}