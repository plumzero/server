#include "mysqlwrapper.h"

MysqlWrapper::MysqlWrapper() {
	mMysql = new MYSQL();
}

MysqlWrapper::~MysqlWrapper() {
	sql_close();
	if (mMysql) free(mMysql);
}

bool MysqlWrapper::sql_connect(std::string host, uint16_t port, std::string user, std::string password, std::string db) {
	if (mysql_init(mMysql)) {
		char flag = 1;
		mysql_options(mMysql, MYSQL_OPT_RECONNECT, (char*)&flag);
		if (mysql_real_connect(mMysql, host.c_str(), user.c_str(), password.c_str(), db.c_str(), port, NULL, 0)) {
			mysql_query(mMysql, "SET NAMES UTF8");
			return true;
		} else {
			sql_close();
			return false;
		}
	}
	
	return false;
}

bool MysqlWrapper::sql_close() {
	mysql_close(mMysql);
	return true;
}

bool MysqlWrapper::sql_change_user(std::string user, std::string password, std::string db) {
	mysql_change_user(mMysql, user.c_str(), password.c_str(), db.c_str());
	return true;
}

bool MysqlWrapper::sql_select_db(std::string db) {
	mysql_select_db(mMysql, db.c_str());
	return true;
}

bool MysqlWrapper::sql_ping() {
	return 0 == mysql_ping(mMysql);
}
/**
 results: [
	rows1 : [ "id" : "1", "name" : "Paul", 	"age" : "32", "address" : "California" ],
	rows2 : [ "id" : "2", "name" : "Allen", "age" : "25", "address" : "Texas" ],
	rows3 : [ "id" : "3", "name" : "Teddy", "age" : "23", "address" : "Norway" ],
	rows4 : [ "id" : "4", "name" : "Mark",  "age" : "25", "address" : "Rich-Mond"]
 ]  
 */
std::vector<std::vector<std::map<std::string, std::string> > > MysqlWrapper::sql_query(std::string sql) {
	std::vector<std::vector<std::map<std::string, std::string> > >  results;
	if (!mysql_query(mMysql, sql.c_str())) {
		MYSQL_RES* result = nullptr;
		while (result = mysql_store_result(mMysql)) {
			std::vector<std::string> key;
			MYSQL_FIELD* field;
			while (field = mysql_fetch_field(result)) {
				key.push_back(field->name);
			}
			
			std::vector<std::map<std::string, std::string> > rows;
			MYSQL_ROW row = nullptr;
			while (row = mysql_fetch_row(result)) {
				std::map<std::string, std::string> kvmap;
				uint32_t num = mysql_num_fields(result);
				uint32_t i;
				for (i = 0; i < num; i++) {
					kvmap[key[i]] = row[i] ? row[i] : "";
				}
				rows.push_back(kvmap);
			}
			results.push_back(rows);
			
			mysql_free_result(result);
			mysql_next_result(mMysql);
		}
	}
	return results;
}
