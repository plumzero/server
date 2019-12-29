#include "mysqlmanager.h"

SqlManager::SqlManager() {
	mSqlWrap = nullptr;
}

SqlManager::~SqlManager() {}

bool SqlManager::sqlmgr_init(const char* config) {
	std::fstream file(config);
	if (file.is_open()) {
		std::ostringstream os;
		os << file.rdbuf();
		file.close();
		
		std::string jstring = os.str();
		
		Json::Reader jReader(Json::Features::strictMode());
		Json::Value jvalue;
		if (jReader.parse(jstring, jvalue)) {
			if (jvalue.isMember("mysql") && jvalue["mysql"].isObject()) {
				Json::Value value = jvalue["mysql"];
				mHost = value.isMember("host") ? value["host"].asString() : "";
				mPort = value.isMember("port") ? value["port"].asUInt() : 0;
				mUser = value.isMember("user") ? value["user"].asString() : "";
				mPassword = value.isMember("password") ? value["password"].asString() : "";
				uint32_t thread = value.isMember("thread") ? value["thread"].asUInt() : 1;
				
				
				MysqlWrapper* mysql = new MysqlWrapper();
				
				if (mysql && mysql->sql_connect(mHost, mPort, mUser, mPassword, "")) {
					mSqlWrap = mysql;
				} else {
					delete mysql;
					mysql = nullptr;
				}
				uint32_t i;
				for (i = 0; i < thread; i++) {
					std::thread query_thread(&SqlManager::sqlmgr_query_thread, this);
					query_thread.detach();
				}
			}
		}
	} else {
		printf("%s %d %s", __FILE__, __LINE__, strerror(errno));
		return false;
	}
	
	return true;
}

void SqlManager::sqlmgr_update(float delay) {}

/**
 * 将 sql 语句及处理该 sql 语句的回调函数放入处理列表
 * @param sql		sql 语句
 * @param callback	sql 回调处理函数
 */
bool SqlManager::sqlmgr_query(char* sql, SqlFunc callback) {
	SqlQuery query;
	query.sql 		= sql;
	query.callback  = callback;
	
	sqlmgr_push_query(query);
	
	return true;
}
/**
 * 调用相应回调函数对 sql 语句进行处理
 * @param sql		sql 语句
 * @param callback	sql 回调处理函数
 */
bool SqlManager::sqlmgr_querysyn(char* sql, SqlFunc callback) {
	if (mSqlWrap) {
		std::vector<std::vector<std::map<std::string, std::string> > > results = mSqlWrap->sql_query(sql);
		
		callback(results);
		
		return true;
	}
	return false;
}

bool SqlManager::sqlmgr_push_query(SqlQuery& query) {
	std::lock_guard<std::mutex> lckgd(mutex_query);
	
	mQueries.push_back(query);
	cond_var.notify_one();	// 不使用 notify_all, 防止多线程竞争同一资源
	
	return true;
}

bool SqlManager::sqlmgr_pop_query(SqlQuery& query) {
	std::lock_guard<std::mutex> lckgd(mutex_query);
	
	if (mQueries.size()) {
		query = mQueries.front();
		mQueries.pop_front();
		
		return true;
	}
	return false;
}

void SqlManager::sqlmgr_query_thread() {
	MysqlWrapper* mysql = new MysqlWrapper();
	
	if (mysql && mysql->sql_connect(mHost, mPort, mUser, mPassword, "")) {
		SqlQuery query;
		while (true) {
			if (false == sqlmgr_pop_query(query)) {		// 无 sql 语句需要处理
				std::unique_lock<std::mutex> lock(mutex_cond);
				cond_var.wait_for(lock, std::chrono::seconds(10));
				continue;
			}
			if (mysql->sql_ping()) {					// 对 sql 语句进行处理
				SqlResults results = mysql->sql_query(query.sql);
				
				set_timer_once(0.0f, [query, results](float dt){ query.callback(results); });
			}
		}
	} else {
		delete mysql;
		mysql = nullptr;
	}
}
