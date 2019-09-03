#pragma once
#include <string>
#include <vector>
using namespace std;

class Bin
{
public:
	Bin(string id, double width, double length, double weight, string station);
	void rotate();
	void update();
	string get_id() const { return this->id; }
	double get_x() const { return x; }
	double get_y() const { return y; }
	double get_length() const { return length; }
	double get_width() const { return width; }
	double get_area() const { return area; }
	double get_weight() const { return weight; }
	double get_real_length() const { return real_length; }
	double get_real_width() const { return real_width; }
	string get_station() const { return station; }
	void set_x(double x) { this->x = x; }
	void set_y(double y) { this->y = y; }	
	void set_real_length(double length) { this->length = length; }
	void set_real_width(double width) { this->width = width; }

private:
	string id;
	double width;
	double length;
	double weight;
	double area;
	string station;
	double x;
	double y;
	double real_length;
	double real_width;
};

Bin::Bin(string id, double width, double length, double weight, string station)
{
	this->id = id;
	this->width = width;
	this->length = length;
	this->real_length = length;
	this->real_width = width;
	this->area = width * length;
	this->weight = weight;
	this->station = station;
	x = 0;
	y = 0;
	if (this->length > this->width)
		this->rotate();
}

inline void Bin::rotate()
{
	swap(this->length, this->width);
}

inline void Bin::update()
{
	real_length = length;
	real_width = width;
}

