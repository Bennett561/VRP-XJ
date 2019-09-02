#include "entity\Vehicle.h"
#include "entity\Bin.h"
#include "entity\Station.h"
#include "VNS.h"
#include "util.h" //工具函数
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include <iostream>
#include <map>

using namespace rapidjson;
using namespace vns;
using namespace my_util;

int main() {
	// 导入数据
	bins = get_bins_data();
	stations = get_stations_data();
	get_distance_matrix();
	get_load_time_matrix();



	//VNS开始
	best_known_sol = vns::initialize();
	best_known_cost = cal_total_cost();
	current_neighbour_cost = best_known_cost;
	last_neighbour_cost = best_known_cost;
	last_neighbour_sol = best_known_sol;

	cout << best_known_cost << endl;;
	n_break = 1;  //当前迭代打散车数
	temperature = 10000; //当前温度，即py中的T[0]
	d_away = D_INTERVAL;  //距下次退火的迭代数，即py中的T[1]
	//cout << best_known_sol << endl;
	//for (int iteration = 0; iteration < 5; iteration++) {
	//	cout << "当前迭代第" << iteration << "次:" << endl;
	//	no_improve_flag = false;
	//	while (!no_improve_flag) {
	//		no_improve_flag = true;
	//		Search(LS1);
	//	}
	//}


	getchar();
	return 0;
}