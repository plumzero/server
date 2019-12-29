#include "servermanager.h"

/////////////////// 工具函数 ///////////////////
/**	
	应用程序 <- input buffer  <- socket
	应用程序 -> output buffer -> socket
 */
/////////////////// 工具函数 ///////////////////
/**
 * 将 evbuf 中的数据读出，恢复，并存入到 msg 中
 */
bool read_buf(evbuffer *evbuf, std::string& msg) {
	if (evbuffer_get_length(evbuf) > PROHEADERSIZE) {
		ProtocolHeader header;
		evbuffer_copyout(evbuf, &header, PROHEADERSIZE);
		// uint8_t version = header.version;
		uint8_t code = ~header.orthocode;
		uint16_t bodysize = ntohs(header.bodysize);
		
		if (evbuffer_get_length(evbuf) >= PROHEADERSIZE + bodysize) {
			evbuffer_drain(evbuf, PROHEADERSIZE);
			
			uint8_t* sbuf = new uint8_t[bodysize];
			int32_t readsize = 0;
			while (readsize < bodysize) {
				int32_t bytes = evbuffer_remove(evbuf, sbuf, bodysize - readsize);
				if (bytes > 0) {
					readsize += bytes;
				} else {
					break;
				}
			}
			std::string data = std::string((const char*)sbuf, readsize);
			delete[] sbuf;
			
			if (data.size() == bodysize) {
				size_t i;
				for (i = 0; i < data.size(); i++) {
					// data[i] ^= code;
				}
				msg = data;
				return true;
			}
		}
	}
	
	evbuffer_drain(evbuf, evbuffer_get_length(evbuf));
	return false;
}
/**
 * 将 msg 中的数据取出，整合，写入到 evbuf 中
 */
bool write_buf(evbuffer* evbuf, std::string msg) {
	ProtocolHeader header;
	memset(&header, 0, PROHEADERSIZE);
	header.version		= 1;
	header.orthocode	= rand() % 0xFF;
	header.bodysize		= htons(msg.size());
	
	uint8_t code = ~header.orthocode;
	size_t i;
	for (i = 0; i < msg.size(); i++) {
		// msg[i] ^= code;
	}
	std::string oput = std::string((const char*)&header, PROHEADERSIZE) + msg;
	
	int ret;
	ret = evbuffer_add(evbuf, oput.c_str(), oput.size());
	return ret == 0;
}
 
/////////////////// 有关客户端的回调函数 ///////////////////
/**
 * 从客户端的 bufferevent 中读取数据，生成字符串风格的消息，并交给相应的服务器进行处理
 * @param	bufev 	客户端的 bufferevent
 * @param	arg		传递的参数，这里是客户端信息(ClientInfo)
 */
void listener_readcb(bufferevent* bufev, void* arg) {
	// ClientInfo* client = (ClientInfo*)arg;
	evbuffer* evbuf = bufferevent_get_input(bufev);
	
	std::string msg;
	while (read_buf(evbuf, msg)) {
		ServerManager::getInstance()->srvmgr_client_message_handle(bufev, msg, arg);
		msg.clear();
	}
}
/**
 * 向客户端缓冲区写入数据，这里是断开客户端。如果客户端缓冲区为空且客户端无未决事件，则断开。
 * @param bufev		客户端缓冲区
 * @param arg		传递的参数，这里是客户端信息(ClientInfo)
 */
void listener_writecb(bufferevent* bufev, void* arg) {
	ClientInfo* client = (ClientInfo*)arg;
	
	evbuffer* evbuf = bufferevent_get_output(bufev);
	// 客户端 buffevent 为空且无 BEV_EVENT_CONNECTED 事件
	if (evbuffer_get_length(evbuf) == 0 && client->events == 0) {
		ServerManager::getInstance()->srvmgr_client_disconnected(bufev, arg);
	}
}
/**
 * 客户端的请求事件(连接事件或非正常事件(错读、错写、未知错误、用户指定超时内连接不成功))
 * @param bufev		客户端缓冲区
 * @param events	客户端发来的事件
 * @param arg		传递的参数，这里是客户端信息(ClientInfo)
 */
void listener_eventcb(bufferevent* bufev, short events, void* arg) {
	ClientInfo* client = (ClientInfo*)arg;
	client->events = events;
	
	if (events & (BEV_EVENT_READING | BEV_EVENT_WRITING | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)) {
		ServerManager::getInstance()->srvmgr_client_disconnected(bufev, arg);
	} else if (events & BEV_EVENT_CONNECTED) {
		ServerManager::getInstance()->srvmgr_client_connected(bufev, arg);
	}
}
/**
 * 处理监听到的（客户端）请求。服务器只会客户端的三种请求事件进行监听并处理，分别是：读事件，写事件，信号事件
 * @param listener	 服务器监听器
 * @param fd		 客户端套接字
 * @param sa		 客户端地址
 * @param socket_len 客户端地址长度
 * @param arg		 传递的参数，这里是服务器的信息(ServerInfo)
 */
void listener_cb(evconnlistener* listener, evutil_socket_t fd, sockaddr* sa, int socket_len, void* arg) {
	ServerInfo* server = (ServerInfo*)arg;
	ClientInfo* client = new ClientInfo();

	sockaddr_in sin;
	memcpy(&sin, sa, socket_len);
	char ipv4_addr[16] = {0};
	inet_ntop(sin.sin_family, &sin.sin_addr, ipv4_addr, sizeof(ipv4_addr));
	client->host = ipv4_addr;
	client->port = ntohs(sin.sin_port);
	client->fd   = fd;
	
	server->clients[fd] = client;
	
	struct event_base* base = ServerManager::get_base();
	// 为该客户端创建一个基于套接字的 bufferevent
	bufferevent* bufev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bufev) {
		fprintf(stderr, "bufferevent_socke_new error\n");
		fflush(stderr);
		return;
	}
	client->bufev = bufev;
	// 为该客户端设置读回调，写回调，事件回调
	bufferevent_setcb(bufev, listener_readcb, listener_writecb, listener_eventcb, client);
	bufferevent_enable(bufev, EV_READ | EV_WRITE);
	// 为该客户端设置连接事件成功标志
	client->events |= BEV_EVENT_CONNECTED;
	// 从 bufferevent 中读取客户端消息
	ServerManager::getInstance()->srvmgr_client_connected(bufev, client);
}
/////////////////// 有关其他服务器的回调函数 ///////////////////
/**
 * 从对端服务器的 buferevent 中读取数据，生成字符串风格的消息，并交给相应的服务器进行处理
 * @param bufev	对端服务器的 bufferevent
 * @param arg	传递的参数，这里是连接的对端服务器信息(ConnectInfo)
 */
void connect_readcb(bufferevent* bufev, void* arg) {
	// ConnectInfo* peer = (ConnectInfo*)arg;
	evbuffer* evbuf = bufferevent_get_input(bufev);
	
	std::string msg;
	while (read_buf(evbuf, msg)) {
		ServerManager::getInstance()->srvmgr_server_message_handle(bufev, msg, arg);
		msg.clear();
	}
}
/**
 * 向对端服务器缓冲区写入数据，这里是断开与对端服务器的连接。如果对端服务器缓冲区为空且对端服务器无未决事件，则断开。
 * @param bufev		对端服务器缓冲区
 * @param arg		传递的参数，这里是连接的对端服务器信息(ConnectInfo)
 */
void connect_writecb(bufferevent* bufev, void* arg) {
	ConnectInfo* peer = (ConnectInfo*)arg;
	evbuffer* evbuf = bufferevent_get_output(bufev);
	if (0 == evbuffer_get_length(evbuf) && 0 == peer->events) {
		ServerManager::getInstance()->srvmgr_server_disconnected(bufev, arg);
	}
}
/**
 * 其他服务器的请求事件(连接事件或非正常事件(错读、错写、未知错误、用户指定超时内连接不成功))
 * @param bufev		对端服务器缓冲区
 * @param events	对端服务器发来的的事件
 * @param arg		传递的参数，这里连接的对端服务器信息(ConnectInfo)
 */
void connect_eventcb(bufferevent* bufev, short events, void* arg) {
	ConnectInfo* info = (ConnectInfo*)arg;
	info->events = events;
	info->fd = bufferevent_getfd(bufev);
	
	if (events & (BEV_EVENT_READING | BEV_EVENT_WRITING | BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)) {
		ServerManager::getInstance()->srvmgr_server_disconnected(bufev, arg);
	} else {
		ServerManager::getInstance()->srvmgr_server_connected(bufev, arg);
	}
}
/**
 * 对端服务器的连接超时回调，连接超时后会调用此回调重连对端服务器
 * @param fd		对端服务器套接字
 * @param events	对端服务器感兴趣的事件
 * @param arg		传递的参数，这里是对端服务器的连接信息(ConnectInfo)
 */
void connect_timer(evutil_socket_t fd, short events, void* arg) {
	ConnectInfo* info = (ConnectInfo*)arg;
	if (info->events & BEV_EVENT_CONNECTED)		// 如果当前服务器正在连接中，则返回
		return;
	if (info->bufev) {
		bufferevent_free(info->bufev);
		info->bufev = nullptr;
	}
	// 重新创建一个基于套接字的 bufferevent
	bufferevent* bufev = bufferevent_socket_new(ServerManager::get_base(), -1, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bufev) {
		fprintf(stderr, "bufferevent_socket_new failed\n");
		fflush(stderr);
		return;
	}
	info->bufev = bufev;
	
	bufferevent_setcb(bufev, connect_readcb, connect_writecb, connect_eventcb, info);
	bufferevent_enable(bufev, EV_READ | EV_WRITE);
	
	// struct sockaddr_in sin;
	// memset(&sin, 0, sizeof(sin));
	// sin.sin_family = AF_INET;
	// inet_pton(AF_INET, info->host.c_str(), &sin.sin_addr);
	// sin.sin_port = htons(info->port);
	// if (bufferevent_socket_connect(bufev, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
	if (bufferevent_socket_connect_hostname(bufev, nullptr, AF_INET, info->host.c_str(), info->port) < 0) {
		fprintf(stderr, "connect %s:%u failed\n", info->host.c_str(), info->port);
		fflush(stderr);
		bufferevent_free(bufev);
		return;
	}
	return;
}
/////////////////// AbstractServer 成员方法 ///////////////////
AbstractServer::AbstractServer() {
	ServerManager::getInstance()->srvmgr_add_server(this);
}

AbstractServer::~AbstractServer() {
	ServerManager::getInstance()->srvmgr_remove_server(this);
}
/////////////////// ServerManager 成员方法 ///////////////////

event_base* ServerManager::mBase = nullptr;

ServerManager::ServerManager() {}

ServerManager::~ServerManager() {
	for (std::list<ServerInfo*>::iterator it = mServersInfo.begin(); it != mServersInfo.end(); it++) {
		if (*it) delete (*it);
	}
	for (std::list<ConnectInfo*>::iterator it = mConnectsInfo.begin(); it != mConnectsInfo.end(); it++) {
		if (*it) delete (*it);
	}
}
/**
 * 初始化配置
 * @param	config	配置文件路径名
 * @return  true - 成功；false - 失败
 */
bool ServerManager::srvmgr_init(const char* config) {
	std::fstream file(config);
	if (file.is_open()) {
		std::ostringstream os;
		os << file.rdbuf();
		file.close();
		std::string jstring = os.str();
		
		Json::Reader jReader(Json::Features::strictMode());
		Json::Value jvalue;

		if (jReader.parse(jstring, jvalue)) {
			if (jvalue.isMember("type") && jvalue.isMember("id")) {
				mType = static_cast<SERVER_TYPE>(jvalue["type"].asUInt());
				mId = static_cast<SERVER_ID>(jvalue["id"].asUInt());
			}
			int32_t i, size;
			if (jvalue.isMember("listen") && jvalue["listen"].isArray()) {
				Json::Value value = jvalue["listen"];
				size = static_cast<int32_t>(value.size());
				for (i = 0; i < size; i++) {
					if (value[i].isMember("host") && value[i].isMember("port")) {
						std::string host = value[i]["host"].asString();
						uint16_t port = value[i]["port"].asUInt();
						srvmgr_listen(host.c_str(), port);
					}
				}
			}
			if (jvalue.isMember("connect") && jvalue["connect"].isArray()) {
				Json::Value value = jvalue["connect"];
				size = static_cast<int32_t>(value.size());
				for (i = 0; i < size; i++) {
					if (value[i].isMember("host") && value[i].isMember("port")) {
						std::string host = value[i]["host"].asString();
						uint16_t port = value[i]["port"].asUInt();
						srvmgr_connect(host.c_str(), port);
					}
				}
			}
		}
	}

	return true;
}
/**
 * 记录服务器信息，为该服务器创建一个监听，并将服务器信息添加入管理
 * @param	host	服务器监听地址
 * @param	port	服务器监听端口
 * @return	true - 成功；false - 失败
 */
bool ServerManager::srvmgr_listen(const char* host, uint16_t port) {
	for (std::list<ServerInfo*>::iterator it = mServersInfo.begin(); it != mServersInfo.end(); it++) {
		if ((*it)->host == host && (*it)->port == port) {
			return true;
		}
	}
	
	ServerInfo* info = new ServerInfo();
	info->host = host;
	info->port = port;
	mServersInfo.push_back(info);
	if (nullptr == mBase) {
		fprintf(stderr, "base is null\n");
		fflush(stderr);
		return false;
	}
	
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	inet_pton(AF_INET, host, &sin.sin_addr);
	sin.sin_port = htons(port);
	// 为该服务器创建一个监听器事件（套接字可重用，启用底层销毁），一旦有消息到达即触发该事件回调
	evconnlistener* listener = evconnlistener_new_bind(mBase, listener_cb, info, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));
	if (NULL == listener) {
		fprintf(stderr, "listen %s:%u error\n", host, port);
		fflush(stderr);
		return false;
	}
	info->listener = listener;
	
	printf("Listen\t%s:%u\n", host, port);
	
	return true;
}
/**
 * 接收来自其他服务器的请求，为对端服务器创建一个对应的 buferevent，并将对端服务器信息添加入管理
 * @param	host	对端服务器地址
 * @param	port	对端服务器端口
 * @return	true - 成功；false - 失败
 */
bool ServerManager::srvmgr_connect(const char* host, uint16_t port) {
	for (std::list<ConnectInfo*>::iterator it = mConnectsInfo.begin(); it != mConnectsInfo.end(); it++) {
		if ((*it)->host == host && (*it)->port == port) {
			return true;
		}
	}
	ConnectInfo* info = new ConnectInfo();
	info->host = host;
	info->port = port;
	mConnectsInfo.push_back(info);
	if (nullptr == mBase) {
		fprintf(stderr, "base is null\n");
		fflush(stderr);
		return false;
	}
	
	bufferevent* bufev = bufferevent_socket_new(mBase, -1, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == bufev) {
		fprintf(stderr, "bufferevent_socket_new failed\n");
		fflush(stderr);
		return false;
	}
	info->bufev = bufev;
	// 为该对端服务器设置读回调，写回调，事件回调
	bufferevent_setcb(bufev, connect_readcb, connect_writecb, connect_eventcb, info);
	bufferevent_enable(bufev, EV_READ | EV_WRITE);
	
	// struct sockaddr_in sin;
	// memset(&sin, 0, sizeof(sin));
	// sin.sin_family = AF_INET;
	// inet_pton(AF_INET, host, &sin.sin_addr);
	// sin.sin_port = htons(port);
	// // 在 buferevent 上启动对对端服务器的连接
	// if (bufferevent_socket_connect(bufev, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
	if (bufferevent_socket_connect_hostname(bufev, nullptr, AF_INET, host, port) < 0) {
		fprintf(stderr, "connect %s:%u failed\n", host, port);
		fflush(stderr);
		bufferevent_free(bufev);
		return false;
	}
	// 为对端服务器创建一个超时事件，超时过后触发此回调
	struct event* timer = event_new(mBase, -1, EV_PERSIST, connect_timer, info);
	struct timeval tv = {5, 0};
	event_add(timer, &tv);
	info->timer = timer;
	
	printf("Connect\t%s:%u\n", host, port);
	return true;
}
/**
 * 将客户端连接请求消息交给相应服务器处理
 * @param	bufev	客户端的 bufferevent
 * @param	arg		传递的参数，这里是客户端信息(ClientInfo)
 *
 * 可以在这个方法中设置负载均衡（多台服务器 -- 服务器管理 -- 多个客户端）
 */
void ServerManager::srvmgr_client_connected(bufferevent* bufev, void* arg) {
	ClientInfo* client = (ClientInfo*)arg;
	printf("Client Connected\t%d\t%s:%u\n", client->fd, client->host.c_str(), client->port);
	
	for (std::set<AbstractServer*>::iterator it = mServers.begin(); it != mServers.end(); it++) {
		if (*it) {
			(*it)->net_client_connected(bufev, arg);
		}
	}
}
/**
 * 将客户端断开请求消息交给相应的服务器处理，并将管理的客户端信息删除
 * @param	bufev	客户端的 bufferevent
 * @param	arg		传递的参数，这里是客户端信息(ClientInfo)
 */
void ServerManager::srvmgr_client_disconnected(bufferevent* bufev, void* arg) {
	ClientInfo* client = (ClientInfo*)arg;
	printf("Client Disconnected\t%d\t%s:%u\n", client->fd, client->host.c_str(), client->port);
	
	for (std::set<AbstractServer*>::iterator it = mServers.begin(); it != mServers.end(); it++) {
		if (*it) {
			(*it)->net_client_disconnected(bufev, arg);
		}
	}
	int fd = client->fd;
	for (std::list<ServerInfo*>::iterator it = mServersInfo.begin(); it != mServersInfo.end(); it++) {
		if ((*it)->clients.find(fd) != (*it)->clients.end() && (*it)->clients[fd] == client) {
			(*it)->clients.erase(fd);
			delete client;
		}
	}
}
/**
 * 将客户端发来的常规消息交给相应的服务器处理
 * @param bufev		客户端的 bufferevent
 * @param msg		字符串风格的消息
 * @param arg		传递的参数，这里是客户端信息(ClientInfo)
 */
void ServerManager::srvmgr_client_message_handle(bufferevent* bufev, std::string msg, void* arg) {
	// ClientInfo* client = (ClientInfo*)arg;
	
	Message message(msg);
	for (std::set<AbstractServer*>::iterator it = mServers.begin(); it != mServers.end(); it++) {
		if (*it) {
			(*it)->net_client_message_handle(bufev, message, arg);
		}
	}
}
/**
 * 将其他服务器的连接请求交给相应的服务器处理
 * @param bufev		对端服务器的 bufev
 * @param arg		传递的参数，这里是对端服务器的连接信息(ConnectInfo)
 */
void ServerManager::srvmgr_server_connected(bufferevent* bufev, void* arg) {
	ConnectInfo* info = (ConnectInfo*)arg;
	printf("Server Connected\t%d\t%s:%u\n", info->fd, info->host.c_str(), info->port);
	
	for (std::set<AbstractServer*>::iterator it = mServers.begin(); it != mServers.end(); it++) {
		if (*it) {
			(*it)->net_server_connected(bufev, arg);
		}
	}
}
/**
 * 将其他服务器的断开请求交给相应的服务器处理
 * @param bufev		对端服务器的 bufev
 * @param arg		传递的参数，这里是对端服务器的连接信息(ConnectInfo)
 */
void ServerManager::srvmgr_server_disconnected(bufferevent* bufev, void* arg) {
	ConnectInfo* info = (ConnectInfo*)arg;
	printf("Server Disconnected\t%d\t%s:%u\n", info->fd, info->host.c_str(), info->port);
	
	for (std::set<AbstractServer*>::iterator it = mServers.begin(); it != mServers.end(); it++) {
		if (*it) {
			(*it)->net_server_disconnected(bufev, arg);
		}
	}
}
/**
 * 将其他服务器发来的常规消息交给相应的服务器处理
 * @param bufev		对端服务器的 bufev
 * @param arg		传递的参数，这里是对端服务器的连接信息(ConnectInfo)
 */
void ServerManager::srvmgr_server_message_handle(bufferevent* bufev, std::string msg, void* arg) {
	// ConnectInfo* peer = (ConnectInfo*)arg;
	
	Message message(msg);
	for (std::set<AbstractServer*>::iterator it = mServers.begin(); it != mServers.end(); it++) {
		if (*it) {
			(*it)->net_server_message_handle(bufev, message, arg);
		}
	}
}
/**
 * 向客户端或其他服务器发送消息
 * @param 	bufev	客户端或其他服务器的 bufferevent
 * @param 	msg		发往客户端或其他服务器的消息
 * @return 	true - 成功；false - 失败
 */
bool ServerManager::srvmgr_send_message(bufferevent* bufev, Message& msg) {
	return write_buf(bufferevent_get_output(bufev), msg.get_string());
}
/**
 * 向某个客户端发送消息。如果 id 为零，则随机选择一个客户端。
 * @param	msg		要发送给客户端的消息
 * @param	type	客户端类型，这里是 SERVER_TYPE_CLIENT
 * @param	id		客户端 id
 */
void ServerManager::srvmgr_send_msg_to_client(Message& msg, SERVER_TYPE type, SERVER_ID id) {
	if (0 == id) {
		std::vector<bufferevent*> bufev_vec;
		for (std::list<ServerInfo*>::iterator it = mServersInfo.begin(); it != mServersInfo.end(); it++) {
			for (std::map<uint32_t, ClientInfo*>::iterator itr = (*it)->clients.begin(); itr != (*it)->clients.end(); itr++) {
				if (itr->second->type == type) {
					bufev_vec.push_back(itr->second->bufev);
				}
			}
		}
		if (bufev_vec.size()) {
			uint32_t rand_num = rand() % bufev_vec.size();
			srvmgr_send_message(bufev_vec[rand_num], msg);
		}
	} else {
		for (std::list<ServerInfo*>::iterator it = mServersInfo.begin(); it != mServersInfo.end(); it++) {
			for (std::map<uint32_t, ClientInfo*>::iterator itr = (*it)->clients.begin(); itr != (*it)->clients.end(); itr++) {
				if (itr->second->type == type && itr->second->id == static_cast<uint32_t>(id)) {
					srvmgr_send_message(itr->second->bufev, msg);
				}
			}
		}
	}
}
/**
 * 向某类型的其他服务器发送消息，如果 id 为零，则随机选择某类型的一个服务器
 * @param	msg		要发送给对端服务器的消息
 * @param	type	对端服务器类型
 * @param	id		对端服务器id
 */
void ServerManager::srvmgr_send_msg_to_server(Message& msg, SERVER_TYPE type, SERVER_ID id) {
	if (SERVER_ID_NONE == id) {
		std::vector<bufferevent*> bufev_vec;
		for (std::list<ConnectInfo*>::iterator it = mConnectsInfo.begin(); it != mConnectsInfo.end(); it++) {
			if ((*it)->type == type) {
				bufev_vec.push_back((*it)->bufev);
			}
		}
		if (bufev_vec.size()) {
			uint32_t rand_num = rand() % bufev_vec.size();
			srvmgr_send_message(bufev_vec[rand_num], msg);
		}
	} else {
		for (std::list<ConnectInfo*>::iterator it = mConnectsInfo.begin(); it != mConnectsInfo.end(); it++) {
			if ((*it)->type == type && (*it)->id == id) {
				srvmgr_send_message((*it)->bufev, msg);
			}
		}
	}
}
/**
 * 向某类型的所有的客户端发消息
 * @param msg	发送的消息
 * @param type	指定的客户端类型
 */
void ServerManager::srvmgr_broadcast_msg_to_client(Message& msg, SERVER_TYPE type) {
	for (std::list<ServerInfo*>::iterator it = mServersInfo.begin(); it != mServersInfo.end(); it++) {
		for (std::map<uint32_t, ClientInfo*>::iterator itr = (*it)->clients.begin(); itr != (*it)->clients.end(); itr++) {
			if (itr->second->type & type) {
				srvmgr_send_message(itr->second->bufev, msg);
			}
		}
	}
}
/**
 * 向某类型的所有服务器发送消息
 * @param msg	发送的消息
 * @param type	指定的服务器类型
 */
void ServerManager::srvmgr_broadcast_msg_to_server(Message& msg, SERVER_TYPE type) {
	for (std::list<ConnectInfo*>::iterator it = mConnectsInfo.begin(); it != mConnectsInfo.end(); it++) {
		if ((*it)->type & type) {
			srvmgr_send_message((*it)->bufev, msg);
		}
	}
}
/**
 * 向管理添加新的服务器
 * @param server 要添加的服务器
 */
void ServerManager::srvmgr_add_server(AbstractServer* server) {
	if (mServers.find(server) == mServers.end()) {
		mServers.insert(server);
	}
}
/**
 * 从管理中删除指定的服务器
 * @param server 要删除的服务器
 */
void ServerManager::srvmgr_remove_server(AbstractServer* server) {
	if (mServers.find(server) != mServers.end()) {
		mServers.erase(server);
	}
}