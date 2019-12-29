#include "message.h"

////////////////////////////// MsgHeader 方法 //////////////////////////////
void MsgHeader::Read(Json::Value& value) {
	parse_json(value, "from_type", from_type);
	parse_json(value, "from_id",   from_id);
	parse_json(value, "to_type",   to_type);
	parse_json(value, "to_id",     to_id);
	parse_json(value, "msg_id",    msg_id);
}

void MsgHeader::Write(Json::Value& value) {
	write_json(value, "from_type", from_type);
	write_json(value, "from_id",   from_id);
	write_json(value, "to_type",   to_type);
	write_json(value, "to_id",     to_id);
	write_json(value, "msg_id",    msg_id);
}

////////////////////////////// Message 方法 //////////////////////////////
void Message::read_from_msg(std::string& msg) {
	Json::Reader jReader(Json::Features::strictMode());
	Json::Value jValue;
	if (jReader.parse(msg, jValue)) {
		if (jValue.isMember("header")) {
			header.Read(jValue["header"]); 
		}
		if (jValue.isMember("payload")) {
			payload = jValue["payload"];
		}
	}
}
void Message::write_to_msg(std::string& msg) {
	Json::Value value;
	if (header.msg_id) {
		header.Write(value["header"]);
	}
	if (!payload.empty()) {
		value["payload"] = payload;
	}
	msg = Json::FastWriter().write(value);
}

std::string Message::get_string()
{
	Json::Value value;

	if (header.msg_id) {
		header.Write(value["header"]);
	}
	if (!payload.empty()) {
		value["payload"] = payload;
	}

	return Json::FastWriter().write(value);
}

////////////////////////////// MessagePump 方法 //////////////////////////////
/**
 * 消息序列化
 * @param	bufev	缓冲区
 * @param	header	消息头
 */
void MessagePump::send_message(bufferevent* bufev, MsgHeader& header) {
	Message msg;
	msg.header 	= header;
	msg.payload	= get_value();

	ServerManager::getInstance()->srvmgr_send_message(bufev, msg);
}
/**
 * 发送消息，包括消息头和有效载荷。缺省时所有消息都发往转发器。
 * @param header	消息头
 * @param type		目的服务器类型
 * @param id		目的服务器id
 *
 * 消息头的具体内容由具体的服务器填写。
 */
void MessagePump::send_message(MsgHeader& header, SERVER_TYPE type, SERVER_ID id) {
	Message msg;
	msg.header 	= header;
	msg.payload	= get_value();
	
	if (SERVER_TYPE_FORWARD == type) {
		ServerManager::getInstance()->srvmgr_send_msg_to_server(msg, type, id);
	} else {
		ServerManager::getInstance()->srvmgr_send_msg_to_client(msg, type, id);
	}
}
/**
 * 向指定类型的所有客户端发送消息
 * @param header	消息头
 * @param type		客户端类型
 */
void MessagePump::broadcast_msg_to_client(MsgHeader& header, SERVER_TYPE type) {
	Message msg;
	msg.header	= header;
	msg.payload = get_value();
	
	ServerManager::getInstance()->srvmgr_broadcast_msg_to_client(msg, type);
}
/**
 * 向某类型的所有服务器发送消息
 * @param header	消息头
 * @param type		对端服务器类型
 */
void MessagePump::broadcast_msg_to_server(MsgHeader& header, SERVER_TYPE type) {
	Message msg;
	msg.header	= header;
	msg.payload = get_value();
	
	ServerManager::getInstance()->srvmgr_broadcast_msg_to_server(msg, type);
}

//////////////////////////////////////// 其他服务器 -> 核心转发器  ////////////////////////////////////////
void MSGPP_SS2S_CONNECTED::Read(Json::Value& value) {
	parse_json(value, "type", 		type);
	parse_json(value, "id",   		id);
}
void MSGPP_SS2S_CONNECTED::Write(Json::Value& value) {
	write_json(value, "type", 		type);
	write_json(value, "id",			id);
}

void MSGPP_SS2S_HEARTBEAT::Read(Json::Value& value) {
	parse_json(value, "cur_time",	cur_time);
}
void MSGPP_SS2S_HEARTBEAT::Write(Json::Value& value) {
	write_json(value, "cur_time",	cur_time);
}

//////////////////////////////////////// 核心转发器 -> 其他服务器  ////////////////////////////////////////
void MSGPP_S2SS_CONNECTED::Read(Json::Value& value) {
	parse_json(value, "type", 		type);
	parse_json(value, "id",   		id);
}
void MSGPP_S2SS_CONNECTED::Write(Json::Value& value) {
	write_json(value, "type", 		type);
	write_json(value, "id",			id);
}

void MSGPP_S2SS_HEARTBEAT::Read(Json::Value& value) {
	parse_json(value, "cur_time",	cur_time);
}
void MSGPP_S2SS_HEARTBEAT::Write(Json::Value& value) {
	write_json(value, "cur_time",	cur_time);
}

//////////////////////////////////////// 客户端 -> 网关  ////////////////////////////////////////
void MSGPP_C2DS_REQUEST_LOGIN::Read(Json::Value& value) {
	parse_json(value, "id", 		id);
	parse_json(value, "name", 		name);
	parse_json(value, "password", 	password);
}

void MSGPP_C2DS_REQUEST_LOGIN::Write(Json::Value& value) {
	write_json(value, "id", 		id);
	write_json(value, "name", 		name);
	write_json(value, "password", 	password);
}

void MSGPP_C2GS_HEARTBEAT::Read(Json::Value& value) {
	parse_json(value, "cur_time",	cur_time);
}
void MSGPP_C2GS_HEARTBEAT::Write(Json::Value& value) {
	write_json(value, "cur_time",	cur_time);
}
//////////////////////////////////////// 网关 -> 客户端  ////////////////////////////////////////
void MSGPP_GS2C_HEARTBEAT::Read(Json::Value& value) {
	parse_json(value, "cur_time",	cur_time);
}
void MSGPP_GS2C_HEARTBEAT::Write(Json::Value& value) {
	write_json(value, "cur_time",	cur_time);
}

//////////////////////////////////////// 客户端 -> 数据库  ////////////////////////////////////////

//////////////////////////////////////// 数据库 -> 客户端  ////////////////////////////////////////
void MSGPP_DS2C_REQUEST_LOGIN::Read(Json::Value& value) {
	parse_json(value, "error_code",		error_code);
}
void MSGPP_DS2C_REQUEST_LOGIN::Write(Json::Value& value) {
	write_json(value, "error_code",		error_code);
}

//////////////////////////////////////// 客户端 -> 业务服务器  ////////////////////////////////////////

//////////////////////////////////////// 业务服务器 -> 客户端  ////////////////////////////////////////
