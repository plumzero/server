#ifndef __MESSAGE_DEFINE_H__
#define __MESSAGE_DEFINE_H__

/**
 *  C	客户端
 *	GS 	网关服务器
 *  FS	转发服务器
 *  DS	数据库
 *  BS	业务服务器
 */

// 其他服务器 -> 转发器
enum MSG_SS2S {
	SS2S_EXAMPLE 				= 11000,
	SS2S_CONNECTED,
	SS2S_HEARTBEAT,
};

// 转发器 -> 其他服务器
enum MSG_S2SS {
	S2SS_EXAMPLE				= 11500,
	S2SS_CONNECTED,
	S2SS_HEARTBEAT,
};

// 客户端 -> 网关
enum MSG_C2GS {
	C2GS_EXAMPLE				= 12000,
	C2GS_HEARTBEAT,
};

// 网关 -> 客户端
enum MSG_GS2C {
	GS2C_EXAMPLE				= 12500,
	GS2C_HEARTBEAT,
};

// 客户端 -> 数据库
enum MSG_C2DS {
	C2DS_EXAMPLE				= 13000,
	C2DS_REQUEST_LOGIN,
	C2DS_REQUEST_LOGOUT,		// xx
	C2DS_GET_XXX,				// xx
	C2DS_SET_XXX,				// xx
};

// 数据库 -> 客户端
enum MSG_DS2C {
	DS2C_EXAMPLE				= 13500,
	DS2C_REQUEST_LOGIN,
};

// 客户端 -> 业务服务器
enum MSG_C2BS {
	C2BS_EXAMPLE				= 14000,
	C2BS_GET_XXX,				// xx
	C2BS_SET_XXX,				// xx
};

// 业务服务器 -> 客户端
enum MSG_BS2C {
	BS2C_EXAMPLE				= 14500,
};


#endif // !__MESSAGE_DEFINE_H__
