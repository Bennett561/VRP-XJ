#include "Vehicle.h"
#include "Bin.h"
#include "Station.h"
#include <iostream>
#include <map>
#include "VNS.h"
#include "util.h" //���ߺ���
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
using namespace rapidjson;



int main() {


	// ��������
	unordered_map<string, Bin> bins = my_util::get_bins_data();
	unordered_map<string, Vehicle> vehicles = my_util::get_vehicles_data();
	unordered_map<string, Station> stations = my_util::get_stations_data();
	double distance_matrix[num_stations][num_stations];  //"s005��s004�ľ���Ϊ��distance_matrix[4-1][5-1] ˳���Ƿ���" 
	double load_time_matrix[num_stations][num_stations]; //"s005��s004������ʱ��Ϊ��load_time_matrix[4-1][5-1]" 
	my_util::get_distance_matrix(distance_matrix);
	my_util::get_load_time_matrix(load_time_matrix);

	////cout << load_time_matrix[4][5] << endl;
	////cout << stations.at(4).get_load_time() << endl;
	vector<Vehicle> used_vehicles;
	unordered_map<string, Vehicle> unused_vehicles;
	vns::initialize(vehicles, unused_vehicles, used_vehicles, bins, stations);
	cout << "��ʼ�ܳɱ�:" << my_util::cal_total_cost(used_vehicles, distance_matrix);


	//vector<Station>::iterator sit = stations.begin();
	//while (sit != stations.end()) {
	//	vector<Bin>::iterator bit = bins.begin();
	//	while (bit != bins.end()) {
	//		if ((*sit).get_id() == (*bit).get_station()) {
	//			(*sit).own_bins.push_back((*bit));
	//		}
	//		bit++;
	//	}
	//	sit++;
	//}

	//cout << stations.at(5).own_bins[2] << endl;

	//initialize();


	getchar();
	return 0;
}