#pragma once
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <functional>
#include <unordered_map>
#include "custom_hash.h"
#include "VNS.h"
#include "entity\Vehicle.h"
#include "entity\Bin.h"
#include "entity\Station.h"
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\allocators.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\encodings.h"

#define NAME(s) { s, sizeof(s) / sizeof(s[0]) - 1, kPointerInvalidIndex }
#define INDEX(i) { #i, sizeof(#i) - 1, i }

using namespace rapidjson;
using namespace std;
const int num_stations = 215;  // 站点数目

extern vector<Vehicle> used_vehicles;
extern unordered_map<string, Vehicle> unused_vehicles;
extern unordered_map<string, Bin> bins;
extern unordered_map<string, Station> stations;

extern unordered_map<pair<string, string>, double, pair_hash> distance_matrix;
extern unordered_map<pair<string, string>, double, pair_hash> load_time_matrix;
namespace my_util {

	//从文件读入到string里
	char* readFileIntoString(const char * filename)
	{
		ifstream f(filename); //taking file as inputstream
		string str;
		if (f) {
			ostringstream ss;
			ss << f.rdbuf(); // reading data
			str = ss.str();
		}
		return _strdup(str.c_str());
	}

	//写文件
	void writeFile(const char *filename, string s) {
		ofstream f(filename);
		f << s;
		f.close();
	}

	unordered_map<string, Bin> get_bins_data() {
		char* bin_json = my_util::readFileIntoString("month3\\bin.json");
		Document d;
		d.Parse(bin_json);
		const Value& a = d["Bin"];
		unordered_map<string, Bin> bins;
		for (SizeType i = 0; i < a.Size(); i++) {// 使用 SizeType 而不是 size_t
			Bin *temp = new Bin(a[i]["bin_id"].GetString(),
				a[i]["bin_width"].GetDouble(),
				a[i]["bin_length"].GetDouble(),
				a[i]["bin_weight"].GetDouble(),
				a[i]["station"].GetString());
			bins.insert({ a[i]["bin_id"].GetString(), *temp });
		}

		return bins;
	}

	unordered_map<string, Vehicle> get_vehicles_data() {
		char* vehicle_json = my_util::readFileIntoString("month3\\vehicle.json");
		Document d;
		d.Parse(vehicle_json);
		const Value& a = d["Vehicle"];
		unordered_map<string, Vehicle> vehicles;
		for (SizeType i = 0; i < a.Size(); i++) {// 使用 SizeType 而不是 size_t
			Vehicle *temp = new Vehicle(a[i]["vehicle_id"].GetString(),
				a[i]["vehicle_width"].GetDouble(),
				a[i]["vehicle_length"].GetDouble(),
				a[i]["vehicle_weight"].GetDouble(),
				a[i]["flag_down_fare"].GetDouble(),
				a[i]["distance_fare"].GetDouble());
			vehicles.insert({ a[i]["vehicle_id"].GetString(), *temp });
		}

		return vehicles;
	}

	unordered_map<string, Station> get_stations_data() {
		char* station_json = my_util::readFileIntoString("month3\\station.json");
		Document d;
		d.Parse(station_json);
		const Value& a = d["Station"];
		unordered_map<string, Station> stations;
		for (SizeType i = 0; i < a.Size(); i++) {// 使用 SizeType 而不是 size_t
			//Station(string id, double limit, double load_time);
			Station *temp = new Station(a[i]["station_id"].GetString(),
				a[i]["vehicle_limit"].GetDouble(),
				a[i]["loading_time"].GetDouble());
			stations.insert({ a[i]["station_id"].GetString(),*temp });
		}

		return stations;
	}

	void get_distance_matrix() {
		char* matrix_json = my_util::readFileIntoString("month3\\matrix.json");
		Document d;
		d.Parse(matrix_json);
		const Value& a = d["Matrix"];
		for (SizeType i = 0; i < a.Size(); i++) {// 使用 SizeType 而不是 size_t
			distance_matrix.insert({ make_pair(a[i]["departure_station_id"].GetString(), a[i]["arrival_station_id"].GetString()), a[i]["distance"].GetDouble() });
		}
		for (auto& s : stations) {
			distance_matrix.insert({ make_pair(s.first, s.first), 0.0 });
		}
	}

	void get_load_time_matrix() {
		char* matrix_json = my_util::readFileIntoString("month3\\matrix.json");
		Document d;
		d.Parse(matrix_json);
		const Value& a = d["Matrix"];
		for (SizeType i = 0; i < a.Size(); i++) {// 使用 SizeType 而不是 size_t
			distance_matrix.insert({ make_pair(a[i]["departure_station_id"].GetString(), a[i]["arrival_station_id"].GetString()), a[i]["time"].GetDouble() });
		}
		for (auto& s : stations) {
			distance_matrix.insert({ make_pair(s.first, s.first), 0.0 });
		}
	}

	//获取数组长度
	template <class T>
	int getArrayLen(T& array)
	{
		return (sizeof(array) / sizeof(array[0]));
	}

	//初始化时选车
	Vehicle& pick_vehicle(vector<Vehicle>& unused_vehicles, Station& station) {
		vector<Vehicle>::iterator vit = unused_vehicles.begin();
		while (vit != unused_vehicles.end()) {
			if (vit->get_length() <= station.get_limit())
				return *(unused_vehicles.erase(vit));
			vit++;
		}
	}

	// 将站点ID(或车辆ID)转化为序号，如"S007" -> 6
	int id_to_num(string id) {
		return stoi(id.substr(1)) - 1;
	}

	//计算路程
	double route_distance(const vector<string>& route) {
		if (route.size() < 2)
			return 0;
		double d = 0;
		for (size_t i = 0; i < route.size() - 1; i++)
		{
			d += distance_matrix.at(make_pair(route.at(i), route.at(i + 1)));
		}
		return d;
	}

	//计算总用时
	double compute_total_time(vector<string>& route) {
		if (route.size() < 1)
			return 0;
		double total_time = 0;
		for (string s : route) {
			total_time += stations.at(s).get_load_time();
		}
		for (size_t i = 0; i < route.size() - 1; i++)
		{
			total_time += load_time_matrix.at(make_pair(route.at(i), route.at(i + 1)));
		}
		return total_time;
	}

	//解析解
	void resolve_sol(char* init_sol_json) {

		unused_vehicles = get_vehicles_data();
		used_vehicles.clear();
		for (auto& s : stations) {
			s.second.pass_vehicles.clear();
		}
		Document d;
		d.Parse(init_sol_json);

		for (auto& m : d.GetObject()) {
			string vid = m.name.GetString();
			Vehicle &v = unused_vehicles.at(vid);
			used_vehicles.push_back(v);
			unused_vehicles.erase(vid);

			used_vehicles.back().visit_order.clear();
			used_vehicles.back().loaded_items.clear();

			for (auto& sid : m.value.GetArray()[0].GetArray()) {
				used_vehicles.back().visit_order.push_back(sid.GetString());
				stations.at(sid.GetString()).pass_vehicles.insert(vid);
			}

			used_vehicles.back().set_loaded_area(0);
			used_vehicles.back().set_loaded_weight(0);
			for (auto& bid : m.value.GetArray()[1].GetArray()) {
				used_vehicles.back().set_loaded_area(bins.at(bid.GetString()).get_area() + used_vehicles.back().get_loaded_area());
				used_vehicles.back().set_loaded_weight(bins.at(bid.GetString()).get_weight() + used_vehicles.back().get_loaded_weight());
				used_vehicles.back().loaded_items.push_back(bid.GetString());
			}

		}
	}

	char* to_char_array(const string s) {
		char* cstr = new char[s.size() + 1];
		strcpy_s(cstr, s.size() + 1, s.c_str());
		return cstr;
	}


	//保存解
	char *save_sol(const char* filename) {
		Document d;
		d.SetObject();
		Document::AllocatorType& allocator = d.GetAllocator();

		for (auto& v : used_vehicles) {
			Value val(kArrayType);

			//visit_order
			Value vo(kArrayType);
			for (auto s : v.visit_order) {
				vo.PushBack(StringRef(to_char_array(s)), allocator);
			}
			val.PushBack(vo, allocator);

			//loaded_items
			Value li(kArrayType);
			for (auto i : v.loaded_items) {
				li.PushBack(StringRef(to_char_array(i)), allocator);
			}
			val.PushBack(li, allocator);
			d.AddMember(StringRef(to_char_array(v.get_id())), val, allocator);
		}

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		d.Accept(writer);
		writeFile(filename, buffer.GetString());

		return to_char_array(buffer.GetString());
	}



	//打印vector
	template <typename T>
	void print_vector(vector<T> v)
	{
		for (auto item : v) {
			cout << item << '\t';
		}
		cout << endl << endl;
	}

	//计算总成本
	double cal_total_cost() {
		double total_cost = 0;
		for (auto& v : used_vehicles) {
			total_cost += v.get_distance_fare() * route_distance(v.visit_order);
			total_cost += v.get_flagdown_fare();
			if (v.get_id() == "V641")
				cout << "V641:" << v.get_distance_fare() * route_distance(v.visit_order) + v.get_flagdown_fare() << endl;
		}
		return total_cost;
	}

	//计算最优路线, TSP
	double compute_tsp(vector<string>& visit_order) {
		vector<string>::iterator it;
		sort(visit_order.begin(), visit_order.end());
		it = unique(visit_order.begin(), visit_order.end());
		int route_size = distance(visit_order.begin(), it);
		visit_order.resize(route_size);
		vector<string> best_order = visit_order;
		double smallest_distance = 1000000000;
		do {
			double dist = route_distance(visit_order);
			if (smallest_distance > dist) {
				smallest_distance = dist;
				best_order = visit_order;
			}
		} while (std::next_permutation(visit_order.begin(), visit_order.end()));
		visit_order = best_order;
		if (compute_total_time(visit_order) <= 600)
			return smallest_distance;
		return -1;
	}

	//计算最优路线, TSP
	double compute_tsp(vector<string>& visit_order,
		string extra_station) {
		vector<string> record_order = visit_order;
		visit_order.push_back(extra_station);
		double smallest_distance = compute_tsp(visit_order);
		if (smallest_distance == -1) {
			visit_order = record_order;
			return -1;
		}
		return smallest_distance;
	}

	int cal_num_bins() {
		int total_num = 0;
		for (auto& v : used_vehicles) {
			total_num += v.loaded_items.size();
		}
		return total_num;
	}
}