#pragma once
#include "Bin.h"
#include "Station.h"
#include <string>
#include <vector>
using namespace std;


class Vehicle
{
public:
	Vehicle(string id, double width, double length, double weight, double flagdown_fare, double distance_fare);
	double get_width() const { return this->width; }
	double get_length() const { return length; }
	double get_area() const { return area; }
	string get_id() const { return this->id;  }
	double get_weight() const { return weight; }
	double get_flagdown_fare() const { return flagdown_fare; }
	double get_distance_fare() const { return distance_fare; }

	double get_loaded_weight() const { return loaded_weight; }
	double get_loaded_area() const { return loaded_area; }
	void set_loaded_weight(double w) { loaded_weight = w; }
	void set_loaded_area(double a) { loaded_area = a; }

	double occupancy();
	vector<string> visit_order;
	vector<Bin> loaded_items;
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


Vehicle::Vehicle(string id, double width, double length, double weight, double flagdown_fare, double distance_fare)
{
	this->id = id;
	this->width = width;
	this->length = length;
	this->area = width * length;
	this->weight = weight;
	this->flagdown_fare = flagdown_fare;
	this->distance_fare = distance_fare;
	this->loaded_area = 0;
	this->loaded_weight = 0;
}


inline double Vehicle::occupancy()
{
	return this->loaded_area / this->area;
}
