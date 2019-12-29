
#include "test_dataparse.h"

void JsonDataTestType::Read(Json::Value& value) {
	parse_json(value, "test_i", 	test_i);
	parse_json(value, "test_ui", 	test_ui);
	parse_json(value, "test_ul", 	test_ul);
	parse_json(value, "test_f", 	test_f);
	parse_json(value, "test_d", 	test_d);
	parse_json(value, "test_str", 	test_str);
}
void JsonDataTestType::Write(Json::Value& value) {
	write_json(value, "test_i", 	test_i);
	write_json(value, "test_ui", 	test_ui);
	write_json(value, "test_ul", 	test_ul);
	write_json(value, "test_f", 	test_f);
	write_json(value, "test_d", 	test_d);
	write_json(value, "test_str", 	test_str);
}

void JsonDataTestInt::Read(Json::Value& value) {
	parse_json(value, "test_vec_int",			test_vec_int);
	parse_json(value, "test_lst_int",			test_lst_int);
	parse_json(value, "test_set_int",			test_set_int);
	parse_json(value, "test_pair_int_int",		test_pair_int_int);
	parse_json(value, "test_map_int_int",		test_map_int_int);
	parse_json(value, "test_multi_int_int",		test_multi_int_int);
	parse_json(value, "test_vec_pair_int_int", 			test_vec_pair_int_int);
	parse_json(value, "test_lst_pair_int_int",			test_lst_pair_int_int);
	parse_json(value, "test_map_int_vec_int",			test_map_int_vec_int);
	parse_json(value, "test_map_int_set_int",			test_map_int_set_int);
	// parse_json(value, "test_map_int_map_int_int",		test_map_int_map_int_int);
	// parse_json(value, "test_multi_int_pair_int_int",	test_multi_int_pair_int_int);
}
void JsonDataTestInt::Write(Json::Value& value) {
	write_json(value, "test_vec_int",			test_vec_int);
	write_json(value, "test_lst_int",			test_lst_int);
	write_json(value, "test_set_int",			test_set_int);
	write_json(value, "test_pair_int_int",		test_pair_int_int);
	write_json(value, "test_map_int_int",		test_map_int_int);
	write_json(value, "test_multi_int_int",		test_multi_int_int);
	write_json(value, "test_vec_pair_int_int",			test_vec_pair_int_int);
	write_json(value, "test_lst_pair_int_int",			test_lst_pair_int_int);
	write_json(value, "test_map_int_vec_int", 			test_map_int_vec_int);
	write_json(value, "test_map_int_set_int",			test_map_int_set_int);
	// write_json(value, "test_map_int_map_int_int",		test_map_int_map_int_int);
	// write_json(value, "test_multi_int_pair_int_int",	test_multi_int_pair_int_int);
}

void JsonDataTestString::Read(Json::Value& value) {
	parse_json(value, "test_vec_str",			test_vec_str);
	parse_json(value, "test_lst_str",			test_lst_str);
	parse_json(value, "test_set_str",			test_set_str);
	parse_json(value, "test_pair_str_str",		test_pair_str_str);
	parse_json(value, "test_map_str_str",		test_map_str_str);
	parse_json(value, "test_multi_str_str",		test_multi_str_str);
	parse_json(value, "test_vec_pair_str_str", 			test_vec_pair_str_str);
	parse_json(value, "test_lst_pair_str_str",			test_lst_pair_str_str);
	parse_json(value, "test_map_str_vec_str",			test_map_str_vec_str);
	parse_json(value, "test_map_str_set_str",			test_map_str_set_str);
	// parse_json(value, "test_map_str_map_str_str",		test_map_str_map_str_str);
	// parse_json(value, "test_multi_str_pair_str_str",	test_multi_str_pair_str_str);
}
void JsonDataTestString::Write(Json::Value& value) {
	write_json(value, "test_vec_str",			test_vec_str);
	write_json(value, "test_lst_str",			test_lst_str);
	write_json(value, "test_set_str",			test_set_str);
	write_json(value, "test_pair_str_str",		test_pair_str_str);
	write_json(value, "test_map_str_str",		test_map_str_str);
	write_json(value, "test_multi_str_str",		test_multi_str_str);
	write_json(value, "test_vec_pair_str_str",			test_vec_pair_str_str);
	write_json(value, "test_lst_pair_str_str",			test_lst_pair_str_str);
	write_json(value, "test_map_str_vec_str", 			test_map_str_vec_str);
	write_json(value, "test_map_str_set_str",			test_map_str_set_str);
	// write_json(value, "test_map_str_map_str_str",		test_map_str_map_str_str);
	// write_json(value, "test_multi_str_pair_str_str",	test_multi_str_pair_str_str);
}

/**
 * 作用：基本类型测试
 */
void dataparse_test_type() {
	printf("test fundamental type(int, unsigned int, float, double, std::string) ...\n");
	
	JsonDataTestType jdt_write, jdt_read;
	Json::Value value;
	// 填充
	jdt_write.test_i		= -10;
	jdt_write.test_ui		= 20;
	jdt_write.test_ul		= 123456789123;
	jdt_write.test_f		= 3.14;
	jdt_write.test_d		= 2.718;
	jdt_write.test_str		= "hello world hello	world";
	// 写到 json 中去
	jdt_write.Write(value);
	// 从 json 中读出
	jdt_read.Read(value);
	// 打印
	std::cout << std::setw(4) << "test_i = " << jdt_read.test_i << std::endl;
	std::cout << std::setw(4) << "test_ui = " << jdt_read.test_ui << std::endl;
	std::cout << std::setw(4) << "test_ul = " << jdt_read.test_ul << std::endl;
	std::cout << std::setw(4) << "test_f = " << jdt_read.test_f << std::endl;
	std::cout << std::setw(4) << "test_d = " << jdt_read.test_d << std::endl;
	std::cout << std::setw(4) << "test_str = " << jdt_read.test_str << std::endl;
	
	printf("test fundamental type ok\n\n");
}

void dataparse_test_int() {
	printf("test contain(vector, list, set, pair, map) related to int ...\n");
	
	JsonDataTestInt jdt_write, jdt_read;
	Json::Value value;
	// 填充
	// vector<int> 型
	int vec_int_help[] = {3, 6, 9, 12, 15};
	std::vector<int> vec_int(vec_int_help, vec_int_help + 5);	
	jdt_write.test_vec_int	= vec_int;
	// list<int> 型
	int lst_int_help[] = {2, 4, 6, 8, 10};
	std::list<int> lst_int(lst_int_help, lst_int_help + 5);
	jdt_write.test_lst_int = lst_int;
	// set<int> 型
	int set_int_help[] = {1, 3, 5, 7, 9};
	std::set<int> set_int(set_int_help, set_int_help + 5);
	jdt_write.test_set_int = set_int;
	// pair<int, int> 型
	jdt_write.test_pair_int_int = std::make_pair(10, 1010);
	// map<int, int> 型
	std::map<int, int> map_int_int;
	map_int_int[10] = 11;
	map_int_int[20] = 21;
	map_int_int[30] = 30;
	jdt_write.test_map_int_int = map_int_int;
	// multimap<int, int> 型
	std::multimap<int, int> multi_int_int;
	multi_int_int.insert(std::pair<int, int>(10, 1001));
	multi_int_int.insert(std::pair<int, int>(10, 1010));
	multi_int_int.insert(std::pair<int, int>(20, 2200));
	multi_int_int.insert(std::pair<int, int>(30, 3003));
	jdt_write.test_multi_int_int = multi_int_int;
	// vector<pair<int, int> > 型
	std::vector<std::pair<int, int> > vec_pair_int_int;
	vec_pair_int_int.push_back(std::pair<int, int>(1, 4880));
	vec_pair_int_int.push_back(std::pair<int, int>(2, 4880));
	vec_pair_int_int.push_back(std::pair<int, int>(4, 12756));
	vec_pair_int_int.push_back(std::pair<int, int>(3, 6796));
	vec_pair_int_int.push_back(std::pair<int, int>(8, 142984));
	vec_pair_int_int.push_back(std::pair<int, int>(7, 120536));
	vec_pair_int_int.push_back(std::pair<int, int>(6, 51118));
	vec_pair_int_int.push_back(std::make_pair(5, 49576));
	jdt_write.test_vec_pair_int_int = vec_pair_int_int;
	// list<pair<int, int> > 型
	std::list<std::pair<int, int> > lst_pair_int_int;
	lst_pair_int_int.push_back(std::pair<int, int>(1, 12345));
	lst_pair_int_int.push_back(std::pair<int, int>(4, 45678));
	lst_pair_int_int.push_back(std::pair<int, int>(7, 78901));
	lst_pair_int_int.push_back(std::pair<int, int>(3, 34567));
	lst_pair_int_int.push_back(std::pair<int, int>(8, 89012));
	lst_pair_int_int.push_back(std::pair<int, int>(2, 23456));
	lst_pair_int_int.push_back(std::pair<int, int>(6, 67890));
	jdt_write.test_lst_pair_int_int = lst_pair_int_int;
	// map<int, vector<int> > 型
	std::map<int, std::vector<int> > map_int_vec_int;
	int map_int_vec_int_help[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	std::vector<int> map_int_vec_int_help1(map_int_vec_int_help, map_int_vec_int_help + 5), map_int_vec_int_help2(map_int_vec_int_help + 3, map_int_vec_int_help + 9);
	map_int_vec_int[2] = map_int_vec_int_help1;
	map_int_vec_int[32] = map_int_vec_int_help2; 
	jdt_write.test_map_int_vec_int = map_int_vec_int;
	// map<int, set<int> > 型
	std::map<int, std::set<int> > map_int_set_int;
	int map_int_set_int_help[] = {11, 11, 21, 22, 31, 31, 41, 51, 61, 71};
	std::set<int> map_int_set_int_help1(map_int_set_int_help, map_int_set_int_help + 5), map_int_set_int_help2(map_int_set_int_help + 3, map_int_set_int_help + 10);
	map_int_set_int[3] = map_int_set_int_help1;
	map_int_set_int[43] = map_int_set_int_help2;
	jdt_write.test_map_int_set_int = map_int_set_int;
	// map<int, map<int, int> > 型
	std::map<int, std::map<int, int> > map_int_map_int_int;
	std::map<int, int> map_int_map_int_int_help1, map_int_map_int_int_help2; 
	map_int_map_int_int_help1[2] = 3;
	map_int_map_int_int_help1[3] = 4;
	map_int_map_int_int_help1[4] = 5;
	map_int_map_int_int_help2[12] = 13;
	map_int_map_int_int_help2[13] = 14;
	map_int_map_int_int_help2[14] = 15;
	map_int_map_int_int[1] = map_int_map_int_int_help1;
	map_int_map_int_int[11] = map_int_map_int_int_help2;
	jdt_write.test_map_int_map_int_int = map_int_map_int_int;
	// std::map<int, pair<int, int> > 型
	std::multimap<int, std::pair<int, int> > multi_int_pair_int_int;
	multi_int_pair_int_int.insert(std::make_pair(10, std::pair<int, int>(100, 1000)));
	multi_int_pair_int_int.insert(std::make_pair(20, std::pair<int, int>(200, 2000)));
	multi_int_pair_int_int.insert(std::make_pair(30, std::pair<int, int>(300, 3000)));
	multi_int_pair_int_int.insert(std::make_pair(20, std::pair<int, int>(200, 2000)));
	multi_int_pair_int_int.insert(std::make_pair(40, std::pair<int, int>(400, 4000)));
	jdt_write.test_multi_int_pair_int_int = multi_int_pair_int_int;
	
	/// 在这里添加测试
	
	// 写到 json 中去
	jdt_write.Write(value);
	// 从 json 中读出
	jdt_read.Read(value);
	
	// 打印
	// vector<int> 型
	std::cout << "vector<int> test:" << std::endl;
	traverse_vec_T(jdt_read.test_vec_int);
	// list<int> 型
	std::cout << "list<int> test:" << std::endl;
	traverse_lst_T(jdt_read.test_lst_int);
	// set<int> 型
	std::cout << "set<int> test:" << std::endl;
	traverse_set_T(jdt_read.test_set_int);
	// pair<int, int> 型
	std::cout << "pair<int, int> test:" << std::endl;
	traverse_pair_T_V(jdt_read.test_pair_int_int);
	// map<int, int> 型
	std::cout << "map<int, int> test:" << std::endl;
	traverse_map_T_V(jdt_read.test_map_int_int);
	// multimap<int, int> 型
	std::cout << "multimap<int, int> test:" << std::endl;
	traverse_multi_T_V(jdt_read.test_multi_int_int);
	// vector<pair<int, int> > 型
	std::cout << "vector<pair<int, int> > test:" << std::endl;
	traverse_vec_pair_T_V(jdt_read.test_vec_pair_int_int);
	// list<pair<int, int> > 型
	std::cout << "list<pair<int, int> > test:" << std::endl;
	traverse_lst_pair_T_V(jdt_read.test_lst_pair_int_int);
	// map<int, vector<int> > 型
	std::cout << "map<int, vector<int> > test:" << std::endl;
	traverse_map_T_vec_V(jdt_read.test_map_int_vec_int);
	// map<int, set<int> > 型
	std::cout << "map<int, set<int> > test:" << std::endl;
	traverse_map_T_set_V(jdt_read.test_map_int_set_int);
	// map<int, map<int, int> > 型
	std::cout << "map<int, map<int, int> > test:" << std::endl;
	traverse_map_K_map_T_V(jdt_read.test_map_int_map_int_int);
	// multimap<int, pair<int, int> > 型
	std::cout << "multimap<int, pair<int, int> > test:" << std::endl;
	traverse_multi_K_pair_T_V(jdt_read.test_multi_int_pair_int_int);
	/// 在这里添加测试
	
	printf("test contain(vector, list, set, pair, map) related to int ok\n\n");
}

void dataparse_test_string() {
	printf("test contain(vector, list, set, pair, map) related to string ...\n");
	
	JsonDataTestString jdt_write, jdt_read;
	Json::Value value;
	// 填充
	// vector<string> 型
	std::vector<std::string> vec_str;
	vec_str.push_back("light red");
	vec_str.push_back("orange");
	vec_str.push_back("egg yellow");
	vec_str.push_back("green");
	vec_str.push_back("cyan");
	vec_str.push_back("orange");
	vec_str.push_back("deep blue");
	vec_str.push_back("egg yellow");
	vec_str.push_back("purple");
	jdt_write.test_vec_str = vec_str;
	// list<string> 型
	std::list<std::string> lst_str;
	lst_str.push_back("light red");
	lst_str.push_back("orange");
	lst_str.push_back("egg yellow");
	lst_str.push_back("green");
	lst_str.push_back("cyan");
	lst_str.push_back("orange");
	lst_str.push_back("deep blue");
	lst_str.push_back("egg yellow");
	lst_str.push_back("purple");
	jdt_write.test_lst_str = lst_str;
	// set<string> 型
	std::set<std::string> set_str;
	set_str.insert("light red");
	set_str.insert("orange");
	set_str.insert("egg yellow");
	set_str.insert("green");
	set_str.insert("cyan");
	set_str.insert("orange");
	set_str.insert("deep blue");
	set_str.insert("egg yellow");
	set_str.insert("purple");
	jdt_write.test_set_str = set_str;
	// pair<string, string> 型
	jdt_write.test_pair_str_str = std::make_pair("my key", "my value	token");
	std::cout << std::setw(4) << jdt_write.test_pair_str_str.first << " : " << jdt_write.test_pair_str_str.second << std::endl;
	// map<string, string> 型
	std::map<std::string, std::string> map_str_str;
	map_str_str["little"] = "orange Mars";
	map_str_str["middle"] = "blue Earth";
	map_str_str["large"] = "brown Jupiter";
	jdt_write.test_map_str_str = map_str_str;
	// multimap<string, string> 型
	std::multimap<std::string, std::string> multi_str_str;
	multi_str_str.insert(std::pair<std::string, std::string>("little", "orange Mars"));
	multi_str_str.insert(std::pair<std::string, std::string>("middle", "blue Earth"));
	multi_str_str.insert(std::pair<std::string, std::string>("little", "unknown Venus"));
	multi_str_str.insert(std::pair<std::string, std::string>("little", "orange Mars"));
	jdt_write.test_multi_str_str = multi_str_str;
	// vector<pair<string, string> > 型
	// { "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus" };
	std::vector<std::pair<std::string, std::string> > vec_pair_str_str;
	vec_pair_str_str.push_back(std::pair<std::string, std::string>("one", "little Mercury"));
	vec_pair_str_str.push_back(std::pair<std::string, std::string>("two", "little Venus"));
	vec_pair_str_str.push_back(std::pair<std::string, std::string>("four", "middle Earth"));
	vec_pair_str_str.push_back(std::pair<std::string, std::string>("three", "little Mars"));
	vec_pair_str_str.push_back(std::pair<std::string, std::string>("eight", "large Jupiter"));
	vec_pair_str_str.push_back(std::pair<std::string, std::string>("seven", "large Saturn"));
	vec_pair_str_str.push_back(std::pair<std::string, std::string>("six", "middle Uranus"));
	jdt_write.test_vec_pair_str_str = vec_pair_str_str;
	// list<pair<string, strig> > 型
	std::list<std::pair<std::string, std::string> > lst_pair_str_str;
	lst_pair_str_str.push_back(std::pair<std::string, std::string>("one", "little Mercury"));
	lst_pair_str_str.push_back(std::pair<std::string, std::string>("two", "little Venus"));
	lst_pair_str_str.push_back(std::pair<std::string, std::string>("four", "middle Earth"));
	lst_pair_str_str.push_back(std::pair<std::string, std::string>("three", "little Mars"));
	lst_pair_str_str.push_back(std::pair<std::string, std::string>("eight", "large Jupiter"));
	lst_pair_str_str.push_back(std::pair<std::string, std::string>("seven", "large Saturn"));
	lst_pair_str_str.push_back(std::pair<std::string, std::string>("six", "middle Uranus"));
	jdt_write.test_lst_pair_str_str = lst_pair_str_str;
	// map<string, vector<string> > 型
	std::map<std::string, std::vector<std::string> > map_str_vec_str;
	const char *map_str_vec_str_help[] = { "one", "one", "three three", "three three", "five five", "five", "seven seven", "seven seven", "nine", "nine" };
	std::vector<std::string> map_str_vec_str_help1(map_str_vec_str_help, map_str_vec_str_help + 5), map_str_vec_str_help2(map_str_vec_str_help + 3, map_str_vec_str_help + 9);
	map_str_vec_str["digitkey1"] = map_str_vec_str_help1;
	map_str_vec_str["digitkey2"] = map_str_vec_str_help2; 
	jdt_write.test_map_str_vec_str = map_str_vec_str;
	// map<string, set<string> > 型
	std::map<std::string, std::set<std::string> > map_str_set_str;
	const char *map_str_set_str_help[] = { "one", "one", "three three", "three three", "five five", "five", "seven seven", "seven seven", "nine", "nine" };
	std::set<std::string> map_str_set_str_help1(map_str_set_str_help, map_str_set_str_help + 5), map_str_set_str_help2(map_str_set_str_help + 3, map_str_set_str_help + 10);
	map_str_set_str["key1"] = map_str_set_str_help1;
	map_str_set_str["key2"] = map_str_set_str_help2;
	jdt_write.test_map_str_set_str = map_str_set_str;
	// map<string, map<string, string> > 型
	std::map<std::string, std::map<std::string, std::string> > map_str_map_str_str;
	std::map<std::string, std::string> map_str_map_str_str_help1, map_str_map_str_str_help2; 
	map_str_map_str_str_help1["one_key"] = "three values";
	map_str_map_str_str_help1["three_key"] = "four values";
	map_str_map_str_str_help1["four_key"] = "five values";
	map_str_map_str_str_help2["twelve_k"] = "thirteen";
	map_str_map_str_str_help2["good"] = "hello world";
	map_str_map_str_str["key1"] = map_str_map_str_str_help1;
	map_str_map_str_str["key2"] = map_str_map_str_str_help2;
	jdt_write.test_map_str_map_str_str = map_str_map_str_str;
	// std::map<string, pair<string, string> > 型
	std::multimap<std::string, std::pair<std::string, std::string> > multi_str_pair_str_str;
	multi_str_pair_str_str.insert(std::make_pair("one", std::pair<std::string, std::string>("one hundred", "one thousand")));
	multi_str_pair_str_str.insert(std::make_pair("two", std::pair<std::string, std::string>("two hundred", "two thousand")));
	multi_str_pair_str_str.insert(std::make_pair("two", std::pair<std::string, std::string>("two hundred", "two thousand")));
	multi_str_pair_str_str.insert(std::make_pair("two", std::pair<std::string, std::string>("two hundred", "two million")));
	multi_str_pair_str_str.insert(std::make_pair("two", std::pair<std::string, std::string>("two million", "two billion")));
	jdt_write.test_multi_str_pair_str_str = multi_str_pair_str_str;
	
	// 写到 json 中去
	jdt_write.Write(value);
	// 从 json 中读出
	jdt_read.Read(value);
	
	// 打印
	// vector<string> 型
	std::cout << "vector<string> test:" << std::endl;
	traverse_vec_T(jdt_read.test_vec_str);
	// list<string> 型
	std::cout << "list<string> test:" << std::endl;
	traverse_lst_T(jdt_read.test_lst_str);
	// set<string> 型
	std::cout << "set<string> test:" << std::endl;
	traverse_set_T(jdt_read.test_set_str);
	// pair<string, string> 型
	std::cout << "pair<string, string> test:" << std::endl;
	traverse_pair_T_V(jdt_read.test_pair_str_str);
	// map<string, string> 型
	std::cout << "map<string, string> test:" << std::endl;
	traverse_map_T_V(jdt_read.test_map_str_str);
	// multimap<string, string> 型
	std::cout << "multimap<string, string> test:" << std::endl;
	traverse_multi_T_V(jdt_read.test_multi_str_str);
	// vector<pair<string, string> > 型
	std::cout << "vector<pair<string, string> > test:" << std::endl;
	traverse_vec_pair_T_V(jdt_read.test_vec_pair_str_str);
	// list<pair<string, string> > 型
	std::cout << "list<pair<string, string> > test:" << std::endl;
	traverse_lst_pair_T_V(jdt_read.test_lst_pair_str_str);
	// map<string, vector<string> > 型
	std::cout << "map<string, vector<string> > test:" << std::endl;
	traverse_map_T_vec_V(jdt_read.test_map_str_vec_str);
	// map<string, set<string> > 型
	std::cout << "map<string, set<string> > test:" << std::endl;
	traverse_map_T_set_V(jdt_read.test_map_str_set_str);
	// map<string, map<string, string> > 型
	std::cout << "map<string, map<string, string> > test:" << std::endl;
	traverse_map_K_map_T_V(jdt_read.test_map_str_map_str_str);
	// multimap<string, pair<string, string> > 型
	std::cout << "multimap<string, pair<string, string> > test:" << std::endl;
	traverse_multi_K_pair_T_V(jdt_read.test_multi_str_pair_str_str);
	
	printf("test contain(vector, list, set, pair, map) related to string ok\n\n");
}