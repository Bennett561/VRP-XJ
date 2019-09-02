#include "entity\Vehicle.h"
#include "entity\Bin.h"
#include "entity\Station.h"
#include "VNS.h"
#include "util.h" //¹¤¾ßº¯Êý
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include <iostream>
#include <map>

using namespace rapidjson;
using namespace vns;
using namespace my_util;


int main() {
	vector<int> a = { 3,5,4 };
	vector<int> &b = a;
	my_util::print_vector(b);
	my_util::print_vector(a);
	b.at(1) = 888;
	my_util::print_vector(b);
	my_util::print_vector(a);


	getchar();
	return 0;
}