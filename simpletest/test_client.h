
#include "../include/include.h"
#include "../message/message.h"
#include "../message/message_define.h"
#include "../support/tools.h"

class Message;

struct Gateway{
	std::string host;
	uint16_t	port;
	
	Gateway() :host("192.168.43.228"), port(6789) {}
	Gateway(std::string host) :host(host) {}
	Gateway(std::string host, uint16_t port) :host(host), port(port) {}
};

// 客户端不再依赖于 libevent 库，也不再依赖于服务器的消息处理机制
/* 
  因为 socket 使用状态机机制，即一个 socket 只能有一种状态（可读，可写，异常），即一次只能处理一种事件。
  
  因此，本测试客户端的设计有两种方案：
	一种是只创建一个套接字和一个可读描述集。测试发送消息时在读回调中完成；
	一种是创建两个（或三个）套接字，分别创建与之对应的可读描述符集，可写描述符集（或异常描述符集），每个
	描述符集专注自己的事件，有各自的回调；
  为了方便，本客户端使用第一种方案。

  为了保证客户端能够得到休息（以使程序能够完全执行完毕），为客户端设置一段等待时间（缺省为  200 millisecond）；
  一般来说，消息不会太大，所以只为每个 socket 分配 2048 字节的堆内存进行读写；
 */
class Socket
{
public:
	Socket(uint32_t id);
	~Socket();
public:
	bool 		skt_create();
	uint32_t 	skt_get();
	void 		skt_setnoblocking();
	bool 		skt_connect(std::string host, uint16_t port);
	bool		skt_login();
public:
	bool 		skt_read_buf(std::string& msg, unsigned char* data, size_t datalen);
	bool 		skt_write_buf(std::string msg, unsigned char* buf, size_t buflen);
public:
	void		skt_message_dispatch(struct timeval tv = { .tv_sec = 0, .tv_usec = 1000 * 1000 });
	void		skt_client_message_handle(unsigned char* data, size_t datalen);		// 客户端发送消息
	void		skt_server_message_handle(unsigned char* data, size_t datalen);		// 处理来自服务器的消息
	
private:
	uint32_t		mId;
	uint32_t 		mSocket;
	unsigned char* 	mBuffer;
	size_t 			mBufsize;	// 为简便，读写缓冲等长
};
