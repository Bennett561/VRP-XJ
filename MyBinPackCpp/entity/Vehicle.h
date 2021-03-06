#pragma once
#include "Bin.h"
#include "Station.h"
#include <string>
#include <vector>
using namespace std;


class Vehicle
{
public:
	Vehicle(string id, double w, double l, double wt, double fd_fare, double dist_fare)
		:id(id), width(w), length(l), weight(wt), flagdown_fare(fd_fare), distance_fare(dist_fare)
	{
		this->area = width * length;
		this->loaded_area = 0;
		this->loaded_weight = 0;
		visit_order.clear();
		loaded_items.clear();
	}
	double get_width() const { return this->width; }
	double get_length() const { return length; }
	double get_area() const { return area; }
	string get_id() const { return this->id; }
	double get_weight() const { return weight; }
	double get_flagdown_fare() const { return flagdown_fare; }
	double get_distance_fare() const { return distance_fare; }

	double get_loaded_weight() const { return loaded_weight; }
	double get_loaded_area() const { return loaded_area; }
	void set_loaded_weight(double w) { loaded_weight = w; }
	void set_loaded_area(double a) { loaded_area = a; }

	double occupancy();
	vector<string> visit_order;
	vector<string> loaded_items;
	bool operator!=(const Vehicle &v);
	bool operator==(const Vehicle &v);
private:
	string id;
	double width;
	double length;
	double area;
	double weight;
	double flagdown_fare;
	double distance_fare;
	double loaded_weight;
	double loaded_area;

};

inline bool Vehicle::operator==(const Vehicle &v)
{
	if (this->id != v.get_id())
	{
		return false;
	}
	return true;
}

inline bool Vehicle::operator!=(const Vehicle &v)
{
	if (this->id != v.get_id())
	{
		return true;
	}
	return false;
}

inline double Vehicle::occupancy()
{
	return this->loaded_area / this->area;
}

