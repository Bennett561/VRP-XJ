#include "Vehicle.h"
#include "Bin.h"
#include "Station.h"
#include "VNS.h"
#include "util.h" //���ߺ���
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include <iostream>
#include <map>
using namespace rapidjson;


const int N_BREAK_MAX = 27;  //����ɢ����
const double D_RATE = 0.97;  //�˻�����
const int D_INTERVAL = 3;    //�˻�����������

int main() {

	// ��������
	unordered_map<string, Bin> bins = my_util::get_bins_data();
	unordered_map<string, Vehicle> vehicles = my_util::get_vehicles_data();
	unordered_map<string, Station> stations = my_util::get_stations_data();
	double distance_matrix[num_stations][num_stations];  //"s005��s004�ľ���Ϊ��distance_matrix[4-1][5-1] ˳���Ƿ���" 
	double load_time_matrix[num_stations][num_stations]; //"s005��s004������ʱ��Ϊ��load_time_matrix[4-1][5-1]" 
	my_util::get_distance_matrix(distance_matrix);
	my_util::get_load_time_matrix(load_time_matrix);

	vector<Vehicle> used_vehicles;
	unordered_map<string, Vehicle> unused_vehicles;

	//VNS��ʼ
	char* best_known_sol = vns::initialize(vehicles, unused_vehicles, used_vehicles, bins, stations);
	double best_known_cost = my_util::cal_total_cost(used_vehicles, distance_matrix);
	double current_neighbour_cost = best_known_cost;
	double last_neighbour_cost = best_known_cost;
	char* last_neighbour_sol = best_known_sol;



	int n_break = 1;  //��ǰ������ɢ����
	double temperature = 10000; //��ǰ�¶ȣ���py�е�T[0]
	int d_away = D_INTERVAL;  //���´��˻�ĵ���������py�е�T[1]

	for (int iteration = 0; iteration < 5; iteration++) {
		cout << "��ǰ������" << iteration << "��:" << endl;
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