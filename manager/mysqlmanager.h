#ifndef __MYSQL_MANAGER_H__
#define __MYSQL_MANAGER_H__

#include "../include/include.h"
#include "../include/singleton.h"
#include "../support/timer.h"
#include "../wrapper/mysqlwrapper.h"

typedef std::vector<std::vector<std::map<std::string, std::string> > > SqlResults;
typedef std::function<void(SqlResults)> SqlFunc;

struct SqlQuery
{
	std::string sql;
	SqlFunc		callback;
};

class SqlManager : public Singleton<SqlManager>, public Timer
{
public:
	SqlManager();
	~SqlManager();
public:
	bool sqlmgr_init(const char* config);
	void sqlmgr_update(float delay);
public:
	template<typename T, typename ...V, typename ...Args>
	bool sqlmgr_query(char* sql, void (T::*method)(SqlResults, V...), T* obj, Args...args);
	bool sqlmgr_query(char* sql, SqlFunc callback);
	template<typename T, typename ...V, typename ...Args>
	bool sqlmgr_querysyn(char* sql, void (T::*method)(SqlResults, V...), T* obj, Args...args);
	bool sqlmgr_querysyn(char* sql, SqlFunc callback);
public:
	bool sqlmgr_push_query(SqlQuery& query);
	bool sqlmgr_pop_query(SqlQuery& query);
	void sqlmgr_query_thread();
private:
	std::string mHost;
	uint16_t	mPort;
	std::string mUser;
	std::string mPassword;
	
	std::list<SqlQuery> mQueries;
	MysqlWrapper* 		mSqlWrap;
	
	std::mutex mutex_query;
	std::mutex mutex_cond;
	std::condition_variable cond_var;
};

template<typename T, typename ...V, typename ...Args>
bool SqlManager::sqlmgr_query(char* sql, void (T::*method)(SqlResults, V...), T* obj, Args...args) {
	SqlFunc callback = std::bind(method, obj, std::placeholders::_1, args...);
	sqlmgr_query(sql, callback);
	return true;
}

template<typename T, typename ...V, typename ...Args>
bool SqlManager::sqlmgr_querysyn(char* sql, void (T::*method)(SqlResults, V...), T* obj, Args...args) {
	SqlFunc callback = std::bind(method, obj, std::placeholders::_1, args...);
	sqlmgr_querysyn(sql, callback);
	return true;
}

#endif // !__MYSQL_MANAGER_H__