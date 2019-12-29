#ifndef __DATA_PARSE_H__
#define __DATA_PARSE_H__

#include "../include/include.h"
#include "../support/tools.h"

/**
 * 本文件只可解析 基本类型及其相关容器(vector, list, set, map)和 JsonDataBase 派生结构体。 本文件中定义了一部分接口，如果需要其他接口，可以继续添加。
 * 本文件无法解析 非 JonDataBase 结构体及其相关容器(如 std::vector<ownDefinedStruct1>, std::map<int, ownDefinedStruct2> 等)。
 * 如果想要实现自定义结构体，需要自己实现，可调用本文件接口。 
 * 
 * 注意：
 *	1. 解析字符串时，键值务必不要使用空白符分隔，如不应 "my key", 而应该 "my_key";
 *  2. 对 64 位有符号和无符号长整型，暂不支持
 *
 *
 * 待实现的数据结构
 *	std::vector<std::pair<int, std::pair<int, int> > >
 *  std::pair<int, std::vector<int> >
 *	std::map<int, std::vector<std::pair<int, int> > >
 */

/**
 * JsonDataBase 是一个提供 json 解析/生成 的虚基类，主要功能函数是：
 *	Read: 	将数据从 json 中解析到相应类型变量中
 *  Write:	将各种类型数据生成 json
 * 相关的辅助功能方法有：
 *	get_value:		写入 json
 *	set_value:		将结构体、类等对象 json 解析到相应类型变量中
 *	get_string		将各种数据类型标识符转化为字符串，写入 json
 *	set_string		将字符串风格的 json 转化为正式的 json, 之后将该 json 解析到相应类型变量中
 */
class JsonDataBase
{
public:
	virtual void Read(Json::Value& value)	= 0;
	virtual void Write(Json::Value& value)	= 0;
	
	virtual Json::Value get_value() {
		Json::Value value;
		Write(value);
		return value;
	}
	
	virtual void set_value(Json::Value& value) {
		Read(value);
	}
	
	virtual std::string get_string() {
		Json::Value value;
		Write(value);
		return Json::FastWriter().write(value);
	}
	
	virtual void set_string(std::string msg) {
		Json::Reader jReader(Json::Features::strictMode());
		Json::Value jValue;
		if (jReader.parse(msg, jValue)) {
			Read(jValue);
		}
	}
};

////////// 基本工具函数 //////////

////////// json 解析部分 - 将 json 类型解析为 结构化数据类型 //////////

/**
 * 作用：json 解析工具函数
 */
template <typename T>
static bool parse_value(Json::Value& value, T& data) {
	if (value.isInt()) {
		convert(value.asInt(), data);
	} else if (value.isUInt()) {
		convert(value.asUInt(), data);
	} else if (value.isDouble()) {
		convert(value.asDouble(), data);
	} else if (value.isString()) {
		convertstr(value.asString(), (std::string&)data);
	} else if (value.isBool()) {
		convert(value.asBool(), data);
	// } else if (value.asUInt64() >= 0 && value.asUInt64() <= (uint64_t)(-1)) {
		// convert(value.asUInt64(), data);
	// } else if (value.asInt64() <= (int64_t)(uint64_t(-1) / 2) && value.asInt64() > -(int64_t)(uint64_t(-1) / 2)) {
		// convert(value.asInt64(), data);
	} else {
		return false;
	}
	return true;
}
/**
 * 作用：json 解析模板函数一
 */
template <typename T>
static bool parse_json(Json::Value& value, T& data, const std::true_type&) {
	if (std::is_base_of<JsonDataBase, T>::value) {
		if (value.isString()) {
			((JsonDataBase *)&data->set_string(value.asString()));
			return true;
		} else if (value.isObject()) {
			((JsonDataBase *)&data->set_value(value));
			return true;
		}
	}
	return false;
}
/**
 * 作用：json 解析模板函数二
 */
template <typename T>
static bool parse_json(Json::Value& value, T& data, const std::false_type&) {
	return parse_value(value, data);
}

////////// json 解析部分 -  接口函数 //////////

/**
 * 作用：接口函数。将 json 解析为 T 类型，此模板函数会根据 T 的类型调用相关解析函数
 *       如果 T 为 class, struct 类型，则调用解析模板函数一
 *		 如果 T 为 union 等非类类型，则调用解析模板函数二
 */
template <typename T>
static bool parse_json(Json::Value& value, T& data) {
	return parse_json(value, data, std::is_class<T>());
}
/**
 * 作用：接口函数。将 json 解析为 string 类型
 */
static bool parse_json(Json::Value& value, std::string& data) {
	return parse_value(value, data);
}
/**
 * 作用：接口函数。将 json 解析为 vector<T> 类型
 */
template <typename T>
static bool parse_json(Json::Value& value, std::vector<T>& data) {
	if (value.isArray()) {
		data.clear();
		int i;
		for (i = 0; i < value.size(); i++) {
			T temp;
			if (parse_json(value[i], temp)) {
				data.push_back(temp);
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 list<T> 类型
 */
template <typename T>
static bool parse_json(Json::Value& value, std::list<T>& data) {
	if (value.isArray()) {
		data.clear();
		int i;
		for (i = 0; i < value.size(); i++) {
			T temp;
			if (parse_json(value[i], temp)) {
				data.push_back(temp);
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 set<T> 类型
 */
template <typename T> 
static bool parse_json(Json::Value& value, std::set<T>& data) {
	if (value.isArray()) {
		data.clear();
		int i;
		for (i = 0; i < value.size(); i++) {
			T temp;
			if (parse_json(value[i], temp)) {
				data.insert(temp);
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 pair<T, V> 类型
 */
template <typename T, typename V>
static bool parse_json(Json::Value& value, std::pair<T, V>& data) {
	if (value.isObject()) {
		data = std::pair<T, V>();
		Json::Value::Members member = value.getMemberNames();
		Json::Value::Members::iterator it = member.begin();
		V temp;
		if (parse_json(value[*it], temp)) {
			// if (std::is_base_of<std::string, T>::value) {
				// data = std::make_pair(get_str<T>(*it), temp);
			// } else {
				data =  std::make_pair(get_value<T>(*it), temp);
			// }
			return true;
		}
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 map<T, V> 类型
 */
template <typename T, typename V>
static bool parse_json(Json::Value& value, std::map<T, V>& data) {
	if (value.isObject()) {
		data.clear();
		Json::Value::Members member = value.getMemberNames();
		for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++) {
			V temp;
			if (parse_json(value[*it], temp)) {
				data[get_value<T>(*it)] = temp;
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 multimap<T, V> 类型
 */
template <typename T, typename V>
static bool parse_json(Json::Value& value, std::multimap<T, V>& data) {
	if (value.isArray()) {
		data.clear();
		int i;
		for (i = 0; i < value.size(); i++) {
			Json::Value::Members member = value[i].getMemberNames();
			for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++) {
				V temp;
				if (parse_json(value[i][*it], temp)) {
					data.insert(std::pair<T, V>(get_value<T>(*it), temp));
				}
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 vector<pair<T, V> > 类型
 */
template <typename T, typename V>
static bool parse_json(Json::Value& value, std::vector<std::pair<T, V> >& data) {
	if (value.isArray()){
		data.clear();
		int i;
		for (i = 0; i < value.size(); i++) {
			// 也可以调用 pair<T, V>类型解析接口
			Json::Value::Members member = value[i].getMemberNames();
			for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++) {
				V temp;
				if (parse_json(value[i][*it], temp)) {
					data.push_back(std::make_pair(get_value<T>(*it), temp));
				}
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 list<pair<T, V> > 类型
 */
template <typename T, typename V>
static bool parse_json(Json::Value& value, std::list<std::pair<T, V> >& data) {
	if (value.isArray()) {
		data.clear();
		int i;
		for (i = 0; i < value.size(); i++) {
			Json::Value::Members member = value[i].getMemberNames();
			for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++) {
				V temp;
				if (parse_json(value[i][*it], temp)) {
					data.push_back(std::make_pair(get_value<T>(*it), temp));
				}
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 map<T, vector<V> >
 */
template <typename T, typename V>
static bool parse_json(Json::Value& value, std::map<T, std::vector<V> >& data) {
	if (value.isObject()) {
		data.clear();
		Json::Value::Members member = value.getMemberNames();
		for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++) {
			std::vector<V> temp_vec;
			if (parse_json(value[*it], temp_vec)) {
				data[get_value<T>(*it)] = temp_vec;
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 map<T, set<V> >
 */
template <typename T, typename V>
static bool parse_json(Json::Value& value, std::map<T, std::set<V> >& data) {
	if (value.isObject()) {
		data.clear();
		Json::Value::Members member = value.getMemberNames();
		for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++) {
			std::set<V> temp_set;
			if (parse_json(value[*it], temp_set)) {
				data[get_value<T>(*it)] = temp_set;
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。将 json 解析为 multimap<K, pair<T, V> > 类型二
 */
template <typename K, typename T, typename V>
static bool parse_json(Json::Value& value, std::multimap<K, std::pair<T, V> >& data) {
	if (value.isObject()) {
		data.clear();
		Json::Value::Members member = value.getMemberNames();
		for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++) {
			if (value[*it].isArray()) {
				int i;
				for (i = 0; i < value[*it].size(); i++) {
					Json::Value::Members mem = value[*it][i].getMemberNames();
					for (Json::Value::Members::iterator itr = mem.begin(); itr != mem.end(); itr++) {
						V temp;
						if (parse_json(value[*it][i][*itr], temp)) {
							data.insert(std::make_pair(get_value<K>(*it), std::make_pair(get_value<T>(*itr), temp)));
						}
					}
				}
			} else {
				return false;
			}
		}
		return true;
	}
	return false;
}
/**
 * 作用：接口函数。如果 value 中有 key 键，则将 key 键对应的值解析到 data 中去
 */
template <typename T>
static bool parse_json(Json::Value& value, std::string key, T& data) {
	if (value.isMember(key)) {
		return parse_json(value[key], data);
	}
	return false;
}
/**
 * 作用：接口函数。解析字符串风格 json 时，启用严格模式，先将其解析为 Json::Value 类型，再解析为结构化数据
 */
template <typename T>
static bool parse_json(std::string& str, T& data) {
	Json::Reader reader(Json::Features::strictMode());
	Json::Value value;
	if (reader.parse(str, value)) {
		return parse_json(value, data);
	}
	return false;
}

////////// json 生成部分 - 将 结构化数据类型 生成为 json //////////

template <typename T>
static void write_json(Json::Value& value, T& data);

/**
 * 作用：将 data 转化为 json，并返回此 json
 */
template <typename T>
static Json::Value get_json_value(T& data) {
	Json::Value value;
	write_json(value, data);
	return value;
}
/**
 * 作用：生成无风格字符串形式的 json
 */
template <typename T>
static std::string get_json_string(T& data) {
	return Json::FastWriter().write(get_json_value(data));
}
/**
 * 作用：json 生成模板函数一
 */
template <typename T>
static void write_json(Json::Value& value, T& data, const std::true_type&) {
	if (std::is_base_of<JsonDataBase, T>::value) {
		value = ((JsonDataBase *)&data)->get_value();
	} else if (std::is_base_of<std::string, T>::value) {
		value = static_cast<std::string>(data);
	}
}

/**
 * 作用：json 生成模板函数二
 */
template <typename T>
static void write_json(Json::Value& value, T& data, const std::false_type&) {
	if (std::is_same<typename std::decay<T>::type, int>::value) {
		value = static_cast<Json::Int>(data);
	} else if (std::is_same<typename std::decay<T>::type, unsigned int>::value) {
		value = static_cast<Json::UInt>(data);
	} else if (std::is_same<typename std::decay<T>::type, long long int>::value) {
		value = static_cast<Json::Int64>(data);
	} else if (std::is_same<typename std::decay<T>::type, unsigned long long int>::value) {
		value = static_cast<Json::UInt64>(data);
	} else if (std::is_same<typename std::decay<T>::type, float>::value) {
		value = static_cast<double>(data);
	} else if (std::is_same<typename std::decay<T>::type, double>::value) {
		value = static_cast<double>(data);
	} else if (std::is_same<typename std::decay<T>::type, bool>::value) {
		value = static_cast<bool>(data);
	}
}

////////// json 生成部分 -  接口函数 //////////

/**
 * 作用：接口函数。将 T 类型转化为 json，此模板函数会根据 T 的类型调用相关解析函数
 *       如果 T 为 class, struct 类型，则调用解析模板函数一
 *		 如果 T 为 union 等非类类型，则调用解析模板函数二
 */
template <typename T>
static void write_json(Json::Value& value, T& data) {
	write_json(value, data, std::is_class<T>());
}
/**
 * 作用：接口函数。将字符串类型 data 转化为 json
 */
static void write_json(Json::Value& value, std::string& data) {
	value = data;
}
/**
 * 作用：接口函数。将 vector<T> 转化为 json
 */
template <typename T>
static void write_json(Json::Value& value, std::vector<T>& data) {
	for (typename std::vector<T>::iterator it = data.begin(); it != data.end(); it++) {
		value.append(get_json_value(*it));
	}
}
/**
 * 作用：接口函数。将 list<T> 转化为 json
 */
template <typename T>
static void write_json(Json::Value& value, std::list<T>& data) {
	for (typename std::list<T>::iterator it = data.begin(); it != data.end(); it++)	{
		value.append(get_json_value(*it));
	}
}
/**
 * 作用：接口函数。将 set<T> 转化为 json
 */
template <typename T>
static void write_json(Json::Value& value, std::set<T>& data) {
	for (typename std::set<T>::iterator it = data.begin(); it != data.end(); it++) {
		value.append(get_json_value(*it));
	}
}
/**
 * 作用：接口函数。将 pair<T, V> 转化为 json
 */
template <typename T, typename V>
static void write_json(Json::Value& value, std::pair<T, V>& data) {
	value[get_string(data.first)] = get_json_value(data.second);
}
/**
 * 作用：接口函数。将 map<T, V> 转化为 json
 */
template <typename T, typename V>
static void write_json(Json::Value& value, std::map<T, V>& data) {
	for (typename std::map<T, V>::iterator it = data.begin(); it != data.end(); it++) {
		value[get_string(it->first)] = get_json_value(it->second);
	}
}
/**
 * 作用：接口函数。将 multimap<T, V> 转化为 json
 */
template <typename T, typename V>
static void write_json(Json::Value& value, std::multimap<T, V>& data) {
	for (typename std::multimap<T, V>::iterator it = data.begin(); it != data.end(); it++) {
		Json::Value temp_value;
		temp_value[get_string(it->first)] = get_json_value(it->second);
		value.append(temp_value);
	}
}
/**
 * 作用：接口函数。将 vector<pair<T, V> > 转化为 json
 */
template <typename T, typename V>
static void write_json(Json::Value& value, std::vector<std::pair<T, V> >& data) {
	for (typename std::vector<std::pair<T, V> >::iterator it = data.begin(); it != data.end(); it++) {
		Json::Value temp_value;
		temp_value[get_string(it->first)] = get_json_value(it->second);
		value.append(temp_value);
	}
}
/**
 * 作用：接口函数。将 list<pair<T, V> > 转化为 json
 */
template <typename T, typename V>
static void write_json(Json::Value& value, std::list<std::pair<T, V> >& data) {
	for (typename std::list<std::pair<T, V> >::iterator it = data.begin(); it != data.end(); it++) {
		Json::Value temp_value;
		temp_value[get_string(it->first)] = get_json_value(it->second);
		value.append(temp_value);
	}
}
/**
 * 作用：接口函数。将 map<T, vector<V> > 转化为 json
 */
template <typename T, typename V>
static void write_json(Json::Value& value, std::map<T, std::vector<V> >& data) {
	for (typename std::map<T, std::vector<T> >::iterator it = data.begin(); it != data.end(); it++) {
		for (typename std::vector<T>::iterator itr = it->second.begin(); itr != it->second.end(); itr++) {
			value[get_string(it->first)].append(get_json_value(*itr));
		}
	}
}
/**
 * 作用：接口函数。将 map<T, set<T> > 转化为 json
 */
template <typename T, typename V>
static void write_json(Json::Value& value, std::map<T, std::set<V> >& data) {
	for (typename std::map<T, std::set<T> >::iterator it = data.begin(); it != data.end(); it++) {
		for (typename std::set<T>::iterator itr = it->second.begin(); itr != it->second.end(); itr++) {
			value[get_string(it->first)].append(get_json_value(*itr));
		}
	}
}
/**
 * 作用：接口函数。将 multimap<K, pair<T, V> > 类型二 转化为 json
 */
template <typename K, typename T, typename V>
static void write_json(Json::Value& value, std::multimap<K, std::pair<T, V> >& data) {
	for (typename std::multimap<K, std::pair<T, V> >::iterator it = data.begin(); it != data.end(); it++) {
		Json::Value temp_value;
		temp_value[get_string(it->second.first)] = get_json_value(it->second.second);
		value[get_string(it->first)].append(temp_value);
	}
}
/**
 * 作用：接口函数。以 key 作为键，data 作为其映射值，写入 json
 */
template <typename T>
static void write_json(Json::Value& value, std::string key, T& data) {
	write_json(value[key], data);
}

////////// JsonDataBase 派生类测试示例 //////////


#endif // !__DATA_PARSE_H__