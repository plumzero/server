#ifndef __TEST_DATAPARSE_H__
#define __TEST_DATAPARSE_H__

#include "../include/include.h"
#include "../dataparse/dataparse.h"

// 测试函数
void dataparse_test_type();

void dataparse_test_int();

void dataparse_test_string();

// 构造模板
// 构造各种结构体时，可能会涉及到 new 和 delete 的使用，较麻烦，暂不使用

// 打印测试工具模板
// vector<T>
template<typename T>
static void traverse_vec_T(std::vector<T>& data) {
	for (typename std::vector<T>::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setiosflags(std::ios::right) << std::setw(4) << *it << ",";
	}
	std::cout << std::endl;
}
// list<T>
template<typename T>
static void traverse_lst_T(std::list<T>& data) {
	for (typename std::list<T>::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setw(4) << *it << ",";
	}
	std::cout << std::endl;
}
// set<T>
template<typename T>
static void traverse_set_T(std::set<T>& data) {
	for (typename std::set<T>::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setw(4) << *it;
	}
	std::cout << std::endl;
}
// pair<T, V>
template<typename T, typename V>
static void traverse_pair_T_V(std::pair<T, V>& data) {
	std::cout << std::setw(4) << data.first << " : " << data.second << std::endl;
}
// map<T, V>
template<typename T, typename V>
static void traverse_map_T_V(std::map<T, V>& data) {
	for (typename std::map<T, V>::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setw(4) << it->first << " : " << it->second << std::endl;
	}
}
// multimap<T, V>
template<typename T, typename V>
static void traverse_multi_T_V(std::multimap<T, V>& data) {
	for (typename std::multimap<T, V>::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setw(4) << it->first << " : " << it->second << std::endl;
	}
}
// vector<pair<T, V> >
template<typename T, typename V>
static void traverse_vec_pair_T_V(std::vector<std::pair<T, V> >& data) {
	for (typename std::vector<std::pair<T, V> >::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setiosflags(std::ios::left) << std::setw(4) << (*it).first << " : " << std::setw(10) << (*it).second << std::endl;
	}
}
// list<pair<T, V> >
template<typename T, typename V>
static void traverse_lst_pair_T_V(std::list<std::pair<T, V> >& data) {
	for (typename std::list<std::pair<T, V> >::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setiosflags(std::ios::left) << std::setw(4) << (*it).first << " : " << std::setw(10) << (*it).second << std::endl;
	}
}
// map<T, vector<V> >
template<typename T, typename V>
static void traverse_map_T_vec_V(std::map<T, std::vector<V> >& data) {
	for (typename std::map<T, std::vector<T> >::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setw(4) << it->first << " : [";
		for (typename std::vector<V>::iterator itr = it->second.begin(); itr != it->second.end(); itr++) {
			std::cout << std::setw(8) << *itr << ",";
		}
		std::cout << std::setw(4) << "]" << std::endl;
	}
}
// map<T, set<V> >
template<typename T, typename V>
static void traverse_map_T_set_V(std::map<T, std::set<V> >& data) {
	for (typename std::map<T, std::set<V> >::iterator it = data.begin(); it !=  data.end(); it++) {
		std::cout << std::setw(4) << it->first << " : [";
		for (typename std::set<V>::iterator itr = it->second.begin(); itr != it->second.end(); itr++) {
			std::cout << std::setw(8) << *itr << ",";
		}
		std::cout << std::setw(4) << "]" << std::endl;
	}
}
// map<K, map<T, V> >
template<typename K, typename T, typename V>
static void traverse_map_K_map_T_V(std::map<K, std::map<T, V> >& data) {
	for (typename std::map<K, std::map<T, V> >::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setw(4) << it->first << " : {" << std::endl;
		for (typename std::map<T, V>::iterator itr = it->second.begin(); itr != it->second.end(); itr++) {
			std::cout << std::setw(8) << itr->first << " : " << itr->second << std::endl;
		}
		std::cout << std::setw(4) << "}" << std::endl;
	}
}
// multimap<K, pair<T, V> >
template<typename K, typename T, typename V>
static void traverse_multi_K_pair_T_V(std::multimap<K, std::pair<T, V> >& data) {
	for (typename std::multimap<K, std::pair<T, V> >::iterator it = data.begin(); it != data.end(); it++) {
		std::cout << std::setw(4) << it->first << " : {" << it->second.first << " : " << it->second.second << "}" << std::endl;
	}
}

class JsonDataTestType : public JsonDataBase		// 基本类型
{
public:
	JsonDataTestType() :test_i(0), test_ui(0), test_ul(0), test_f(0), test_d(0) {}
	JsonDataTestType(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	int32_t				test_i;
	uint32_t 			test_ui;
	uint64_t			test_ul;
	float 				test_f;
	double 				test_d;
	std::string 		test_str;
};

class JsonDataTestInt : public JsonDataBase		// int 型关联容器
{
public:
	JsonDataTestInt() {}
	JsonDataTestInt(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	std::vector<int> 		test_vec_int;
	std::list<int> 			test_lst_int;
	std::set<int>			test_set_int;
	std::pair<int, int>		test_pair_int_int;
	std::map<int, int> 		test_map_int_int;
	std::multimap<int, int>	test_multi_int_int;
	std::vector<std::pair<int, int> > 		test_vec_pair_int_int;
	std::list<std::pair<int, int> >			test_lst_pair_int_int;
	std::map<int, std::vector<int> >		test_map_int_vec_int;
	std::map<int, std::set<int> >			test_map_int_set_int;
	std::map<int, std::map<int, int> > 		test_map_int_map_int_int;
	std::multimap<int, std::pair<int, int> >	test_multi_int_pair_int_int;
};

class JsonDataTestString : public JsonDataBase		// std::string 型关联容器
{
public:
	JsonDataTestString() {}
	JsonDataTestString(Json::Value& value) { Read(value); }
	virtual void Read(Json::Value& value);
	virtual void Write(Json::Value& value);
public:
	std::vector<std::string> 		test_vec_str;
	std::list<std::string> 			test_lst_str;
	std::set<std::string>			test_set_str;
	std::pair<std::string, std::string>		test_pair_str_str;
	std::map<std::string, std::string> 		test_map_str_str;
	std::multimap<std::string, std::string>	test_multi_str_str;
	std::vector<std::pair<std::string, std::string> > 		test_vec_pair_str_str;
	std::list<std::pair<std::string, std::string> >			test_lst_pair_str_str;
	std::map<std::string, std::vector<std::string> >		test_map_str_vec_str;
	std::map<std::string, std::set<std::string> >			test_map_str_set_str;
	std::map<std::string, std::map<std::string, std::string> > 			test_map_str_map_str_str;
	std::multimap<std::string, std::pair<std::string, std::string> >	test_multi_str_pair_str_str;
};

/// 其他类型的关联容器有空再测

#endif // !__TEST_DATAPARSE_H__