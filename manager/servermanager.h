#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__

#include "../include/include.h"
#include "../include/singleton.h"
#include "../message/message.h"

#define PROHEADERSIZE		sizeof(ProtocolHeader)

class Message;

/**
 * 客户端信息
 * @member type			客户端类型
 * @member id			客户端 id (id != 0)
 * @member fd			客户端套接字描述符
 * @member host			客户端 IP 地址
 * @member port			客户端端口
 * @member events		客户端感兴趣的事件
 * @member bufev		客户端缓冲区
 * @member timer		客户端定时器事件
 */
struct ClientInfo
{
	CLIENT_TYPE 	type;
	CLIENT_ID 		id;
	uint32_t		fd;
	std::string		host;
	uint16_t		port;
	int16_t			events;
	bufferevent*	bufev;
	event*			timer;
	
	ClientInfo() :type(SERVER_TYPE_NONE), id(0), fd(0), port(0), events(0), bufev(nullptr), timer(nullptr){}
	~ClientInfo() { if (bufev) bufferevent_free(bufev); if (timer) event_free(timer); }
};
/**
 * 服务端所保存的来自其他服务器的连接信息
 * @member type			监听服务器类型
 * @member id			监听服务器 id (id != 0)
 * @member fd			交互套接字描述符
 * @member host			监听服务器 IP 地址
 * @member port			监听服务器端口
 * @member events		感兴趣的事件
 * @member bufev		交互缓冲区
 * @member timer		定时器事件
 */
struct ConnectInfo
{
	SERVER_TYPE		type;
	SERVER_ID		id;
	uint32_t		fd;
	std::string		host;
	uint16_t		port;
	uint16_t		events;
	bufferevent*	bufev;
	event*			timer;
	
	ConnectInfo() :type(SERVER_TYPE_NONE), id(SERVER_ID_NONE), fd(0), port(0), events(0), bufev(nullptr), timer(nullptr){}
	~ConnectInfo() { if (bufev) bufferevent_free(bufev); if (timer) event_free(timer); }
};
/**
 * 服务器信息
 * @member host		服务器地址
 * @member port		服务器监听端口
 * @member listener 服务器在该监听端口上的 TCP 连接监听器
 * @member clients  服务器在该监听端口上所维护管理的客户端 	<客户端套接字描述符 : 客户端信息>
 */
struct ServerInfo
{
	std::string						host;
	uint16_t						port;
	evconnlistener*					listener;
	std::map<uint32_t, ClientInfo*>	clients;
	
	ServerInfo() :port(0), listener(nullptr) {}
	~ServerInfo() {
		if (listener) evconnlistener_free(listener);
		for (std::map<uint32_t, ClientInfo*>::iterator it = clients.begin(); it != clients.end(); it++) {
			delete it->second;
		}
	}
};
/**
 * 作用：抽象服务器类。嵌合于上层服务器和底层网络之间，负责底层数据流与上层服务器之间的连接和消息流转。
 */
class AbstractServer
{
public:
	AbstractServer();
	~AbstractServer();
public:
	virtual void net_client_connected(bufferevent* bufev, void* arg)										= 0;
	virtual void net_client_disconnected(bufferevent* bufev, void* arg)										= 0;
	virtual void net_client_message_handle(bufferevent* bufev, Message& msg, void* arg)						= 0;
	
	virtual void net_server_connected(bufferevent* bufev, void* arg)										= 0;
	virtual void net_server_disconnected(bufferevent* bufev, void* arg)										= 0;
	virtual void net_server_message_handle(bufferevent* bufev, Message& msg, void* arg)						= 0;
};
/**
 * 作用：服务器管理类。对消息流进行处理，将写缓冲内的消息序列化并发送，或者将读缓冲内的消息接收并结构化。
 *       此类用于保存和管理 抽象类，实体服务器类，客户端 三个对象的信息。
 * @member mBase			事件集
 * @member mType			消息流来源的服务器类型
 * @member mId				消息流来源的服务器 id
 * @member mServers			所管理的服务器对象集合
 * @member mServersInfo		集合中的服务器信息
 * @member mConnectsInfo	所管理的服务器与对端之间的连接信息
 */
class ServerManager :public Singleton<ServerManager>
{
public:
	ServerManager();
	~ServerManager();
public:
	// 处理客户端的连接
	bool srvmgr_listen(const char* host, uint16_t port);
	// 处理来自其他服务器的连接
	bool srvmgr_connect(const char* host, uint16_t port);
public:
	bool srvmgr_init(const char* config);
	static void set_base(struct event_base* base) { mBase = base; }
	static struct event_base* get_base() { return mBase; }
public:
	// 处理来自客户端的消息
	void srvmgr_client_connected(bufferevent* bufev, void* arg);
	void srvmgr_client_disconnected(bufferevent* bufev, void* arg);
	void srvmgr_client_message_handle(bufferevent* bufev, std::string msg, void* arg);
	// 处理来自其他服务器的消息
	void srvmgr_server_connected(bufferevent* bufev, void* arg);
	void srvmgr_server_disconnected(bufferevent* bufev, void* arg);
	void srvmgr_server_message_handle(bufferevent* bufev, std::string msg, void* arg);
public:
	bool srvmgr_send_message(bufferevent* bufev, Message& msg);
	// 向客户端发送消息
	void srvmgr_send_msg_to_client(Message& msg, SERVER_TYPE type, SERVER_ID id);
	void srvmgr_broadcast_msg_to_client(Message& msg, SERVER_TYPE type);
	// 向其他服务器发送消息
	void srvmgr_send_msg_to_server(Message& msg, SERVER_TYPE type, SERVER_ID id);
	void srvmgr_broadcast_msg_to_server(Message& msg, SERVER_TYPE type);
public:
	// 增加或减少服务器
	void srvmgr_add_server(AbstractServer* server);
	void srvmgr_remove_server(AbstractServer* server);
private:
	static struct event_base* 	mBase;
	SERVER_TYPE					mType;
	SERVER_ID					mId;
	std::set<AbstractServer*>	mServers;
	std::list<ServerInfo*>		mServersInfo;
	std::list<ConnectInfo*>		mConnectsInfo;
};

#endif // !__SERVER_MANAGER_H__
