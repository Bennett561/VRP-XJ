#include "entity\Vehicle.h"
#include "entity\Bin.h"
#include "entity\Station.h"
#include "VNS.h"
#include "util.h" //���ߺ���
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include <iostream>
#include <map>

using namespace rapidjson;
using namespace vns;
using namespace my_util;

int main() {

	// ��������
	bins = get_bins_data();
	stations = get_stations_data();
	get_distance_matrix();
	get_load_time_matrix();



	//VNS��ʼ
	best_known_sol = vns::initialize();
	best_known_cost = cal_total_cost();
	current_neighbour_cost = best_known_cost;
	last_neighbour_cost = best_known_cost;
	last_neighbour_sol = best_known_sol;

	//cout << "195-198:" << distance_matrix.at(make_pair("S195", "S198"))<< endl;
	cout << best_known_cost;
	n_break = 1;  //��ǰ������ɢ����
	temperature = 10000; //��ǰ�¶ȣ���py�е�T[0]
	d_away = D_INTERVAL;  //���´��˻�ĵ���������py�е�T[1]

	for (int iteration = 0; iteration < 5; iteration++) {
		cout << "��ǰ������" << iteration << "��:" << endl;
		no_improve_flag = false;
		while (!no_improve_flag) {
			no_improve_flag = true;
			Search(LS1);
		}
	}


	getchar();
	return 0;
}