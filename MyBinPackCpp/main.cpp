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
#include <cmath>


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

	cout << "初始成本：" << best_known_cost << endl;
	n_break = 1;  //当前迭代打散车数
	temperature = 5000; //当前温度，即py中的T[0]
	d_away = D_INTERVAL;  //距下次退火的迭代数，即py中的T[1]


	//主迭代
	for (int iteration = 0; iteration < 1000; iteration++) {
		cout << "当前迭代第" << iteration << "次:" << endl;
		cout << "当前最优成本：" << best_known_cost << endl;
		cout << "当前打散数：" << n_break << endl; 
		no_improve_flag = false;
		while (!no_improve_flag) {
			no_improve_flag = true;
			Search(LS1);
			Search(LS2);
			Search(LS3);
			Search(LS2);
		}

		cout << "Current Neighbor Best: " << current_neighbour_cost << endl;
		if ((double)(rand() % 10000) / 10000 < exp((last_neighbour_cost - current_neighbour_cost) / temperature)) {
			cout << "solution accepted." << endl;
			n_break = 1;
			last_neighbour_sol = save_sol("result\\last_neighbour1");
			last_neighbour_cost = current_neighbour_cost;
			jump_neighbour();
		}
		else {
			cout << "solution refused." << endl;
			n_break++;
			if (n_break == N_BREAK_MAX)
				n_break = 1;
			resolve_sol(last_neighbour_sol);
			jump_neighbour();
		}
		d_away > 0 ? d_away-- : d_away = 5;
		if (d_away == 0)
			temperature < 0.05 ? temperature : temperature *= D_RATE;
		cout << "当前温度: " << temperature << endl;
		current_neighbour_cost = cal_total_cost();
		cout << "jump后成本:" << current_neighbour_cost << endl;

		tabuset1.clear();
		tabuset2.clear();
		tabuset3.clear();
	}

	getchar();
	system("pause");
	return 0;
}