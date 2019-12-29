#ifndef __MYSQL_WRAPPER_H__
#define __MYSQL_WRAPPER_H__

#include "../include/include.h"

class MysqlWrapper
{
public:
	MysqlWrapper();
	~MysqlWrapper();
public:
	bool sql_connect(std::string host, uint16_t port, std::string user, std::string password, std::string db);
	bool sql_close();
	bool sql_change_user(std::string user, std::string password, std::string db);
	bool sql_select_db(std::string db);
	bool sql_ping();
	std::vector<std::vector<std::map<std::string, std::string> > > sql_query(std::string sql);
private:
	MYSQL* mMysql;
};

#endif // !__MYSQL_WRAPPER_H__