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
private:
	string id;
	double limit;
	double load_time;

};

Station::Station(string id, double limit, double load_time)
{
	this->id = id;
	this->limit = limit;
	this->load_time = load_time;
}
