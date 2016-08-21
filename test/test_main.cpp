#include <iostream>

using namespace std;

#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#include "inipaser.h"

#pragma comment(lib,"rabbitmq.4.lib")
#pragma comment(lib,"ws2_32.lib")


int main()
{
	IniParser inipaser;
	inipaser.parser("../conf.ini");
	inipaser.save("../conf1.ini");
	system("pause");
	return 0;
}