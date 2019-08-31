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
	//			Vehicle vehicle = my_util::pick_vehicle(unused_vehicles, station);
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
		char* init_sol_json = my_util::readFileIntoString("result\\month3init_sol.json");
		my_util::resolve_sol(init_sol_json, vehicles, unused_vehicles, used_vehicles, bins, stations);
		return init_sol_json;
	}

	bool move1() {
		tabuset1.insert(make_tuple("a", "b", "c"));

		return true;
	}
}
