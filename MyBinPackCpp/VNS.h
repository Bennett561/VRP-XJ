#pragma once
#include "Vehicle.h"
#include "Bin.h"
#include "Station.h"
#include "util.h" //工具函数
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include <iostream>
#include <tuple>
#include <unordered_set>
using namespace rapidjson;
using namespace my_util;

vector<Vehicle> used_vehicles;
unordered_map<string, Vehicle> unused_vehicles;

const int N_BREAK_MAX = 27;  //最大打散车数
const double D_RATE = 0.97;  //退火速率
const int D_INTERVAL = 3;    //退火间隔迭代次数

char* best_known_sol;
double best_known_cost;
double current_neighbour_cost;
double last_neighbour_cost;
char* last_neighbour_sol;

int n_break;  //当前迭代打散车数
double temperature; //当前温度，即py中的T[0]
int d_away;  //距下次退火的迭代数，即py中的T[1]
bool no_improve_flag; //是否有改进标识符

struct tuple_hash3 {
	template <class T1, class T2, class T3>
	std::size_t operator () (const std::tuple<T1, T2, T3> &p) const {
		auto h1 = std::hash<T1>{}(get<0>(p));
		auto h2 = std::hash<T2>{}(get<1>(p));
		auto h3 = std::hash<T3>{}(get<2>(p));

		return h1 ^ h2 ^ h3;
	}
};

struct tuple_hash4 {
	template <class T1, class T2, class T3, class T4>
	std::size_t operator () (const std::tuple<T1, T2, T3, T4> &p) const {
		auto h1 = std::hash<T1>{}(get<0>(p));
		auto h2 = std::hash<T2>{}(get<1>(p));
		auto h3 = std::hash<T3>{}(get<2>(p));
		auto h4 = std::hash<T4>{}(get<3>(p));

		return h1 ^ h2 ^ h3 ^ h4;
	}
};

unordered_set<tuple<string, string, string>, tuple_hash3> tabuset1;
unordered_set<tuple<string, string, string, string>, tuple_hash4> tabuset2;
unordered_set<tuple<string, string, string>, tuple_hash3> tabuset3;

namespace vns
{
	//初始化
	//void initialize(vector<Station>& stations, vector<Vehicle> unused_vehicles) {
	//	//初始分配
	//	for each(Station station in stations) {
	//		int index = station.own_bins.size();
	//		while (index > 0)
	//		{
	//			Vehicle vehicle = pick_vehicle(unused_vehicles, station);
	//			station.pass_vehicles.insert(vehicle.get_id());
	//			double total_area = 0;
	//			double total_weight = 0;
	//			//to do:bin packing
	//			for (int i = index - 1; i > -1; i--) {
	//				total_area += 
	//			}
	//		}
	//	}
	//}

	//	return bins;
	//}
	char* initialize(unordered_map<string, Vehicle>& vehicles,
		unordered_map<string, Vehicle>& unused_vehicles,
		vector<Vehicle>& used_vehicles,
		const unordered_map<string, Bin>& bins,
		unordered_map<string, Station> stations) {
		char* init_sol_json = readFileIntoString("result\\month3init_sol.json");
		resolve_sol(init_sol_json, vehicles, unused_vehicles, used_vehicles, bins, stations);
		return init_sol_json;
	}


	void update_tabusets(Vehicle v1, Vehicle v2) {
		auto it1 = tabuset1.begin();
		while (it1 != tabuset1.end()) {
			if (get<0>(*it1) == v1.get_id() || get<1>(*it1) == v1.get_id() \
				|| get<0>(*it1) == v2.get_id() || get<1>(*it1) == v2.get_id()) {
				it1 = tabuset1.erase(it1);
			}
			else {
				it1++;
			}
		}

		auto it2 = tabuset2.begin();
		while (it2 != tabuset2.end()) {
			if (get<0>(*it2) == v1.get_id() || get<1>(*it2) == v1.get_id() \
				|| get<0>(*it2) == v2.get_id() || get<1>(*it2) == v2.get_id()) {
				it2 = tabuset2.erase(it2);
			}
			else {
				it2++;
			}
		}

		auto it3 = tabuset3.begin();
		while (it3 != tabuset3.end()) {
			if (get<0>(*it3) == v1.get_id() || get<1>(*it3) == v1.get_id() \
				|| get<0>(*it3) == v2.get_id() || get<1>(*it3) == v2.get_id()) {
				it3 = tabuset3.erase(it3);
			}
			else {
				it3++;
			}
		}
	}

	bool move1(Station& s,
		Vehicle& v1,
		Vehicle& v2,
		unordered_map<string, Bin>& bins,
		double distance_matrix[][num_stations],
		double load_time_matrix[][num_stations],
		const vector<Station>& stations) {
		if (tabuset1.find(make_tuple(s.get_id(), v1.get_id(), v2.get_id())) != tabuset1.end())
			return false;
		vector<string> items1;
		for (string item : v1.loaded_items) {
			if (s.get_id() == bins.at(item).get_station())
				items1.push_back(item);
		}

		vector<string> items;
		merge(items1.begin(), items1.end(), v2.loaded_items.begin(), v2.loaded_items.end(), back_inserter(items));
		double total_area = 0, total_weight = 0;
		for (string bid : items) {
			total_area += bins.at(bid).get_area();
			total_weight += bins.at(bid).get_weight();
		}

		if (total_area < v2.get_area() && total_weight < v2.get_weight()) {
			//to do 装箱
			if (true) {
				double cost = current_neighbour_cost;
				//移出V1节省的成本
				vector<string> &route = v1.visit_order;
				int route_size = route.size();
				if (route_size == 1)
					cost -= v1.get_flagdown_fare();
				else if (distance(route.begin(), find(route.begin(), route.end(), s.get_id())) == 0)
					cost -= v1.get_distance_fare * distance_matrix[id_to_num(route[1]), id_to_num(route[0])];
				else if (distance(route.begin(), find(route.begin(), route.end(), s.get_id())) == route_size - 1)
					cost -= v1.get_distance_fare * distance_matrix[id_to_num(route[route_size - 1]), id_to_num(route[route_size - 2])];
				else {
					int s_idx = distance(route.begin(), find(route.begin(), route.end(), s.get_id()));
					cost -= v1.get_distance_fare() * (distance_matrix[id_to_num(route[s_idx])][id_to_num(route[s_idx - 1])] +\
						distance_matrix[id_to_num(route[s_idx + 1])][id_to_num(route[s_idx])] -\
						distance_matrix[id_to_num(route[s_idx + 1])][id_to_num(route[s_idx - 1])]);
				}

				//移入V2增加的成本
				double pre_distance = route_distance(v2.visit_order, distance_matrix);
				double smallest_distance = compute_tsp(v2.visit_order, s.get_id(), distance_matrix, load_time_matrix, stations);
				if (smallest_distance != -1){
					cost += (smallest_distance - pre_distance) * v2.get_distance_fare();
					if (cost < current_neighbour_cost) {
						//to do 装箱
						update_tabusets(v1, v2);
						no_improve_flag = false;
						current_neighbour_cost = cost;
						cout << "New Neighbour Best:" << cost << endl;
						v1.visit_order.erase(find(v1.visit_order.begin(), v1.visit_order.end(), s.get_id()));
						for (auto item : items1) {
							v1.loaded_items.erase(find(v1.loaded_items.begin(), v1.loaded_items.end(), item));
						}
						v1.set_loaded_area(v1.get_loaded_area() - total_area + v2.get_loaded_area());
						v1.set_loaded_weight(v1.get_loaded_weight() - total_weight + v2.get_loaded_weight());
						//v2.loaded_items  从装箱中导出
						v2.set_loaded_area(total_area);
						v2.set_loaded_weight(total_weight);
						if (s.pass_vehicles.find(v1.get_id()) != s.pass_vehicles.end())
							s.pass_vehicles.erase(v1.get_id());
						s.pass_vehicles.insert(v2.get_id());
						if (v1.visit_order.empty()) {
							unused_vehicles.insert({ v1.get_id(), v1 });
							used_vehicles.erase(find(used_vehicles.begin(), used_vehicles.end(), v1));
						}
						if (cost < best_known_cost) {
							cout << "New Best Cost:" << cost << endl;
							best_known_cost = cost;
							//to do save sol
							//best_known_sol = save_sol("init_sol");
						}
						return true;
					}
				}
				
			}
		}
		tabuset1.insert(make_tuple(v1.get_id(), v2.get_id(), s.get_id()));
		return true;
	}
}
