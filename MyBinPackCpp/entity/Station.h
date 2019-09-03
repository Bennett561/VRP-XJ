#pragma once
#include "Bin.h"
#include <string>
#include <vector>
#include <set>
using namespace std;

class Station
{
public:
	Station(string id, double limit, double load_time);
	double get_limit() const { return limit; }
	double get_load_time() const { return load_time; }
	string get_id() const { return id; }
	vector<Bin> own_bins;   //该站点所有箱子
	set<string> pass_vehicles;
	void discard(string vid); //从pass_vehicles中去除箱子
private:
	string id;
	double limit;
	double load_time;

};

inline void Station::discard(string vid) { //从pass_vehicles中去除箱子
	if (pass_vehicles.find(vid) != pass_vehicles.end())
		pass_vehicles.erase(vid);
}

Station::Station(string id, double limit, double load_time)
{
	this->id = id;
	this->limit = limit;
	this->load_time = load_time;
}
