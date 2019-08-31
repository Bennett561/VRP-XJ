#pragma once
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <functional>
#include <unordered_map>
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
using namespace rapidjson;
using namespace std;
const int num_stations = 215;  // 站点数目


namespace my_util{

	
	//从文件读入到string里
	char* readFileIntoString(char * filename)
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

	unordered_map<string, Bin> get_bins_data(){
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

	void get_distance_matrix(double distance_matrix[num_stations][num_stations]) {
		char* matrix_json = my_util::readFileIntoString("month3\\matrix.json");
		Document d;
		d.Parse(matrix_json);
		const Value& a = d["Matrix"];
		for (SizeType i = 0; i < a.Size(); i++) {// 使用 SizeType 而不是 size_t
			distance_matrix[i / num_stations][i % num_stations] = a[i]["distance"].GetDouble();
		}
		for (size_t i = 0; i < num_stations; i++){
			distance_matrix[i][i] = 0.0;
		}
	}

	void get_load_time_matrix(double load_time_matrix[num_stations][num_stations]) {
		char* matrix_json = my_util::readFileIntoString("month3\\matrix.json");
		Document d;
		d.Parse(matrix_json);
		const Value& a = d["Matrix"];
		for (SizeType i = 0; i < a.Size(); i++) {// 使用 SizeType 而不是 size_t
			load_time_matrix[i / num_stations][i % num_stations] = a[i]["time"].GetDouble();
		}
		for (size_t i = 0; i < num_stations; i++) {
			load_time_matrix[i][i] = 0.0;
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
	double route_distance(vector<string>& route, double *distance_matrix[num_stations]) {
		double d = 0;
		for(size_t i = 0; i <route.size() - 1; i++)
		{
			d += distance_matrix[id_to_num(route.at(i + 1))][id_to_num(route.at(i))];
		}
		return d;
	}

	//计算总用时
	double compute_total_time(vector<string>& route, double *load_time_matrix[num_stations], vector<Station> stations) {
		double total_time = 0;

		for each(string s in route) {
			total_time += stations.at(id_to_num(s)).get_load_time();
		}
		for (size_t i = 0; i <route.size() - 1; i++)
		{
			total_time += load_time_matrix[id_to_num(route.at(i + 1))][id_to_num(route.at(i))];
		}
		return total_time;
	}

	//计算把j 并入i之后的最佳路线及成本的节省
	void compute_route(Vehicle i, Vehicle j, double *distance_matrix[num_stations]) {
		double cost_ori = j.get_flagdown_fare() + j.get_distance_fare() * route_distance(j.visit_order, distance_matrix) + \
			i.get_distance_fare() * route_distance(i.visit_order, distance_matrix);
		vector<string> temp_vo = i.visit_order;
		temp_vo.insert(temp_vo.end(), j.visit_order.begin(), j.visit_order.end());
		unique(temp_vo.begin(), temp_vo.end());
	}

	//为车辆比较定义key函数
	typedef function<bool(pair<string, Vehicle>, pair<string, Vehicle>)> VehicleComparator;
	//定义按面积比较的key
	VehicleComparator cmp_by_area = 
		[](pair<string, Vehicle> elem1, pair<string, Vehicle> elem2) {
		return elem1.second.get_area() < elem2.second.get_area();
	};


	//解析解
	void resolve_sol(char* init_sol_json, 
		unordered_map<string, Vehicle>& vehicles, 
		unordered_map<string, Vehicle>& unused_vehicles, 
		vector<Vehicle>& used_vehicles, 
		const unordered_map<string, Bin>& bins,
		unordered_map<string, Station> stations) {

		unused_vehicles = vehicles;
		used_vehicles.clear();
		for (auto& s : stations) {
			s.second.pass_vehicles.clear();
		}
		Document d;
		d.Parse(init_sol_json);
		for (auto& m : d.GetObject()) {
			string vid = m.name.GetString();
			Vehicle &v = vehicles.at(vid);
			used_vehicles.push_back(v);
			unused_vehicles.erase(m.name.GetString());
			v.visit_order.clear();

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

	//打印vector
	template <typename T>
	void print_vector(vector<T> v)
	{
		for (auto item : v) {
			cout << item << '\t';
		}
	}

}