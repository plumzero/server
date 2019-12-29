#include "tserver.h"

TServer::TServer() {
	mType 	= SERVER_TYPE_NONE;
	mId		= SERVER_ID_NONE;
	set_timer(0.1f, &TServer::srv_update, this);		// 每隔 0.1s 调用一次 update 方法 
}
TServer::~TServer() {
	kill_timer("update");			// 不太清楚如何根据 update 字符串删除定时器
}
/**
 * 初始化服务器
 */
bool TServer::srv_init(const char* config) {
	return true;
}
/**
 * 服务器更新
 */
void TServer::srv_update(float delay) {}
/**
 * 客户端连接
 */
void TServer::net_client_connected(bufferevent* bufev, void* arg) {}
/**
 * 客户端断开
 */
void TServer::net_client_disconnected(bufferevent* bufev, void* arg) {}
/**
 * 处理客户器消息
 */
void TServer::net_client_message_handle(bufferevent* bufev, Message& msg, void* arg) {}
/**
 * 其他服务器连接
 */
void TServer::net_server_connected(bufferevent* bufev, void* arg) {}
/**
 * 其他服务器断开
 */
void TServer::net_server_disconnected(bufferevent* bufev, void* arg) {}
/**
 * 处理其他服务器消息
 */
void TServer::net_server_message_handle(bufferevent* bufev, Message& msg, void* arg) {}