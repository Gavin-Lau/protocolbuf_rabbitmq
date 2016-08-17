#include <iostream>

using namespace std;

#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#pragma comment(lib,"rabbitmq.4.lib")
#pragma comment(lib,"ws2_32.lib")

void send(amqp_socket_t *socket)
{
	amqp_connection_state_t conn;
	conn = amqp_new_connection();

	socket = amqp_tcp_socket_new(conn);
	if (!socket) {
		printf("creating TCP socket");
	}

	int status = amqp_socket_open(socket, "127.0.0.1", 5672);
	if (status) {
		printf("opening TCP socket");
	}

	amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "gavin", "lgk");
	amqp_channel_open(conn, 1);

	
		amqp_basic_properties_t props;
		props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
		props.content_type = amqp_cstring_bytes("text/plain");
		props.delivery_mode = 2; /* persistent delivery mode */
		amqp_basic_publish(conn, 1, amqp_cstring_bytes("test"), amqp_cstring_bytes("test"),
							0, 0, &props, amqp_cstring_bytes("hello world"));
	

	amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
	amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
	amqp_destroy_connection(conn);
}

int main()
{
	amqp_socket_t *socket = NULL;

	send(socket);
	system("pause");
	return 0;
}