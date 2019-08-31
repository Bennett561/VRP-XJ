#pragma once
#include "Vehicle.h"
#include "Bin.h"
#include "Station.h"
#include <iostream>
#include "util.h" //工具函数
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
using namespace rapidjson;



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
	void initialize(unordered_map<string, Vehicle>& vehicles, 
		unordered_map<string, Vehicle>& unused_vehicles, 
		vector<Vehicle>& used_vehicles,
		const unordered_map<string, Bin>& bins,
		unordered_map<string, Station> stations) {
		char* init_sol_json = my_util::readFileIntoString("result\\month3init_sol.json");
		my_util::resolve_sol(init_sol_json, vehicles, unused_vehicles, used_vehicles, bins, stations);
	}

	bool move1() {
		return true;
	}
}
