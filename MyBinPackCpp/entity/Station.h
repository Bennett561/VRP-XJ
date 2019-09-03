#pragma once
#include "Bin.h"
#include <string>
#include <vector>
#include <set>
using namespace std;

class Station
{
public:
	Station(string id, double limit, double load_time)
		:id(id), limit(limit), load_time(load_time){}

	double get_limit() const { return limit; }
	double get_load_time() const { return load_time; }
	string get_id() const { return id; }
	vector<Bin> own_bins;   //��վ����������
	set<string> pass_vehicles;
	void discard(string vid); //��pass_vehicles��ȥ������
private:
	string id;
	double limit;
	double load_time;

};

inline void Station::discard(string vid) { //��pass_vehicles��ȥ������
	if (pass_vehicles.find(vid) != pass_vehicles.end())
		pass_vehicles.erase(vid);
}

