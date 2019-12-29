#include "../include/include.h"
#include "test_dataparse.h"
#include "test_client.h"

int main(int argc, char* argv[])
{
	// 数据解析测试
	dataparse_test_type();
	dataparse_test_int();
	dataparse_test_string();
	
	// 客户端测试
	// Socket* client = new Socket(1);
	// client->skt_create();
	// client->skt_login();
	// client->skt_connect("192.168.43.228", 6789);
	// client->skt_message_dispatch();
	
	return 0;
}
