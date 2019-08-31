#include "Vehicle.h"
#include "Bin.h"
#include "Station.h"
#include <iostream>
#include <map>
#include "util.h" //工具函数
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
using namespace rapidjson;



int main() {
	vector<int> a = { 3,3,4,5 };
	vector<int> b = a;
	b.at(0) = 7;
	cout << "变了吗？" << a.at(0) << endl;
	cout << "变了吗？" << b.at(0) << endl;

	// 导入数据
	//vector<Bin> bins = my_util::get_bins_data();  //bins[i] 即为 "B(i+1)"
	//vector<Vehicle> vehicles = my_util::get_vehicles_data();
	//vector<Station> stations = my_util::get_stations_data();
	//double distance_matrix[num_stations][num_stations];  //"s005到s004的距离为：distance_matrix[4-1][5-1] 顺序是反的" 
	//double load_time_matrix[num_stations][num_stations]; //"s005到s004的运输时间为：load_time_matrix[4-1][5-1]" 
	//my_util::get_distance_matrix(distance_matrix);
	//my_util::get_load_time_matrix(load_time_matrix);

	////cout << load_time_matrix[4][5] << endl;
	////cout << stations.at(4).get_load_time() << endl;
	//vector<Vehicle> used_vehicles;
	//vector<Vehicle> unused_vehicles = vehicles;

	//
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



	getchar();
	return 0;
}