#include "Vehicle.h"
#include "Bin.h"
#include "Station.h"
#include "VNS.h"
#include "util.h" //工具函数
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include <iostream>
#include <map>
using namespace rapidjson;


const int N_BREAK_MAX = 27;  //最大打散车数
const double D_RATE = 0.97;  //退火速率
const int D_INTERVAL = 3;    //退火间隔迭代次数

int main() {

	// 导入数据
	unordered_map<string, Bin> bins = my_util::get_bins_data();
	unordered_map<string, Vehicle> vehicles = my_util::get_vehicles_data();
	unordered_map<string, Station> stations = my_util::get_stations_data();
	double distance_matrix[num_stations][num_stations];  //"s005到s004的距离为：distance_matrix[4-1][5-1] 顺序是反的" 
	double load_time_matrix[num_stations][num_stations]; //"s005到s004的运输时间为：load_time_matrix[4-1][5-1]" 
	my_util::get_distance_matrix(distance_matrix);
	my_util::get_load_time_matrix(load_time_matrix);

	vector<Vehicle> used_vehicles;
	unordered_map<string, Vehicle> unused_vehicles;

	//VNS开始
	char* best_known_sol = vns::initialize(vehicles, unused_vehicles, used_vehicles, bins, stations);
	double best_known_cost = my_util::cal_total_cost(used_vehicles, distance_matrix);
	double current_neighbour_cost = best_known_cost;
	double last_neighbour_cost = best_known_cost;
	char* last_neighbour_sol = best_known_sol;



	int n_break = 1;  //当前迭代打散车数
	double temperature = 10000; //当前温度，即py中的T[0]
	int d_away = D_INTERVAL;  //距下次退火的迭代数，即py中的T[1]

	for (int iteration = 0; iteration < 5; iteration++) {
		cout << "当前迭代第" << iteration << "次:" << endl;
		bool no_improve_flag = false;
		while (!no_improve_flag) {
			no_improve_flag = true;
			vns::move1();
			//cout << tabuset1.size() << endl;
		}
	}


	getchar();
	return 0;
}