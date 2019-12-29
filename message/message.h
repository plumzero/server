#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "../include/include.h"
#include "../dataparse/dataparse.h"
#include "../manager/servermanager.h"

/**
 * 服务器类型和服务器ID存在于消息体中，此时可能需要进行JSON转换。为了方便，要使用 build-in 类型，而不是新类型。
 */

/**
 * 自定义消息头
 * @member from_type	源服务器类型
 * @member from_id		源服务器 id
 * @member to_type		目标服务器类型
 * @member to_id		目标服务器 id
 * @member msg_id		发送的消息 id
 */
class MsgHeader : public JsonDataBase
{
public:
	MsgHeader() 
		:from_type(0), from_id(0), to_type(0), to_id(0), msg_id(0) { }
	MsgHeader(const uint32_t msg_id)														// 广播给所有服务器的消息
		:from_type(0), from_id(0), to_type(0), to_id(0), msg_id(msg_id) { }
	MsgHeader(const uint32_t to_type, const uint32_t to_id, const uint32_t msg_id)			// 向指定服务器发送消息
		:from_type(0), from_id(0), to_type(to_type), to_id(to_id), msg_id(msg_id) { }
	MsgHeader(const uint32_t from_type, const uint32_t from_id, const uint32_t to_type, const uint32_t to_id, const uint32_t msg_id)
		:from_type(from_type), from_id(from_id), to_type(to_type), to_id(to_id), msg_id(msg_id) {}
public:
	void Read(Json::Value& value);
	void Write(Json::Value& value);
public:
	uint32_t from_type;
	uint32_t from_id;
	uint32_t to_type;
	uint32_t to_id;
	uint32_t msg_id;
};
/**
 * 消息：包括消息头(MsgHeader) 和 有效载荷(MSG_*2*_...)
 * @member
 * @member
 * @member
 */
class Message		// 不继承任何类，作为一个单独的包装类使用
{
public:
	Message() {}
	Message(std::string& msg) { read_from_msg(msg); }
public:
	void read_from_msg(std::string& msg);
	void write_to_msg(std::string& msg);
	std::string get_string();
public:
	MsgHeader 		header;
	Json::Value		payload;
};
/**
 * 将 MessagePump 定义为带虚函数的类
 */
class MessagePump
{
public:
	virtual void Read(Json::Value& value) 		= 0;
	virtual void Write(Json::Value& value)		= 0;

	Json::Value get_value() {
		Json::Value value;
		Write(value);
		return value;
	}
	
	std::string get_string() {
		Json::Value value;
		Write(value);
		return Json::FastWriter().write(value);
	}
	
public:
	void send_message(bufferevent* bufev, MsgHeader& header);
	void send_message(MsgHeader& header, SERVER_TYPE type = SERVER_TYPE_FORWARD, SERVER_ID id = SERVER_ID_NONE);
	void broadcast_msg_to_client(MsgHeader& header, SERVER_TYPE type = SERVER_TYPE_ALL);
	void broadcast_msg_to_server(MsgHeader& header, SERVER_TYPE type = SERVER_TYPE_ALL);
};

//////////////////////////////////////// 其他服务器 -> 核心转发器  ////////////////////////////////////////
class MSGPP_SS2S_CONNECTED : public MessagePump
{
public:
	MSGPP_SS2S_CONNECTED() :type(0), id(0) {}
	MSGPP_SS2S_CONNECTED(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	uint32_t 	type;
	uint32_t	id;
};

class MSGPP_SS2S_HEARTBEAT : public MessagePump
{
public:
	MSGPP_SS2S_HEARTBEAT() :cur_time(0) {}
	MSGPP_SS2S_HEARTBEAT(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	uint64_t 	cur_time;
};


//////////////////////////////////////// 核心转发器 -> 其他服务器  ////////////////////////////////////////
class MSGPP_S2SS_CONNECTED : public MessagePump
{
public:
	MSGPP_S2SS_CONNECTED() :type(0), id(0) {}
	MSGPP_S2SS_CONNECTED(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	uint32_t 	type;
	uint32_t	id;
};
class MSGPP_S2SS_HEARTBEAT : public MessagePump
{
public:
	MSGPP_S2SS_HEARTBEAT() :cur_time(0) {}
	MSGPP_S2SS_HEARTBEAT(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	uint64_t 	cur_time;
};

//////////////////////////////////////// 客户端 -> 网关  ////////////////////////////////////////
class MSGPP_C2DS_REQUEST_LOGIN : public MessagePump		// 用于模拟客户端登录用
{
public:
	MSGPP_C2DS_REQUEST_LOGIN() : id(0) {}
	MSGPP_C2DS_REQUEST_LOGIN(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	uint32_t	id;
	std::string name;
	std::string password;
};

class MSGPP_C2GS_HEARTBEAT : public MessagePump
{
public:
	MSGPP_C2GS_HEARTBEAT() :cur_time() {}
	MSGPP_C2GS_HEARTBEAT(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	uint64_t	cur_time;
};

//////////////////////////////////////// 网关 -> 客户端  ////////////////////////////////////////
class MSGPP_GS2C_HEARTBEAT : public MessagePump
{
public:
	MSGPP_GS2C_HEARTBEAT() :cur_time() {}
	MSGPP_GS2C_HEARTBEAT(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	uint64_t	cur_time;
};


//////////////////////////////////////// 客户端 -> 数据库  ////////////////////////////////////////

//////////////////////////////////////// 数据库 -> 客户端  ////////////////////////////////////////
class MSGPP_DS2C_REQUEST_LOGIN : public MessagePump
{
public:
	MSGPP_DS2C_REQUEST_LOGIN() :error_code(0) {}
	MSGPP_DS2C_REQUEST_LOGIN(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	int32_t 	error_code;
};

//////////////////////////////////////// 客户端 -> 业务服务器  ////////////////////////////////////////

//////////////////////////////////////// 业务服务器 -> 客户端  ////////////////////////////////////////

#endif	// !__MESSAGE_H__