#pragma once
#include "entity\Vehicle.h"
#include "entity\Bin.h"
#include "entity\Station.h"
#include "bpp.h"
#include "custom_hash.h"
#include "util.h" //工具函数
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include <iostream>
#include <tuple>
#include <unordered_set>

using namespace rapidjson;
using namespace my_util;

vector<Vehicle> used_vehicles;
unordered_map<string, Bin> bins;
unordered_map<string, Vehicle> unused_vehicles;
unordered_map<string, Station> stations;
unordered_map<pair<string, string>, double, pair_hash> distance_matrix;
unordered_map<pair<string, string>, double, pair_hash> load_time_matrix;

const int print_freq = 600;  // Local Search输出间隔

const int N_BREAK_MAX = 27;  //最大打散车数
const double D_RATE = 0.97;  //退火速率
const int D_INTERVAL = 3;    //退火间隔迭代次数

char* best_known_sol;
double best_known_cost;
double current_neighbour_cost;
double last_neighbour_cost;
char* last_neighbour_sol;

int n_break;  //当前迭代打散车数
double temperature; //当前温度，即py中的T[0]
int d_away;  //距下次退火的迭代数，即py中的T[1]
bool no_improve_flag; //是否有改进标识符

unordered_set<tuple<string, string, string>, tuple_hash3> tabuset1;
unordered_set<tuple<string, string, string, string>, tuple_hash4> tabuset2;
unordered_set<tuple<string, string, string>, tuple_hash3> tabuset3;

namespace vns
{
	//初始化
	char* initialize() {
		char* init_sol_json = readFileIntoString("result\\month3init_sol.json");
		resolve_sol(init_sol_json);
		return init_sol_json;
	}

	template <typename T>
	void update_single_tabuset(Vehicle v1, Vehicle v2, T tabuset) {
		auto it = tabuset.begin();
		while (it != tabuset.end()) {
			if (get<0>(*it) == v1.get_id() || get<1>(*it) == v1.get_id() \
				|| get<0>(*it) == v2.get_id() || get<1>(*it) == v2.get_id()) {
				it = tabuset.erase(it);
			}
			else {
				it++;
			}
		}
	}


	void update_tabusets(Vehicle v1, Vehicle v2) {
		update_single_tabuset(v1, v2, tabuset1);
		update_single_tabuset(v1, v2, tabuset2);
		update_single_tabuset(v1, v2, tabuset3);
	}

	bool move1(Station& s,
		Vehicle& v1,
		Vehicle& v2) {

		if (tabuset1.find(make_tuple(v1.get_id(), v2.get_id(), s.get_id())) != tabuset1.end())
			return false;
		vector<string> items1;
		vector<string> items1_left;
		for (string item : v1.loaded_items) {
			if (s.get_id() == bins.at(item).get_station())
				items1.push_back(item);
			else
				items1_left.push_back(item);
		}
		//cout << "----------before------------" << endl;
		//cout << "v1 num: " << v1.loaded_items.size() << endl;
		//cout << "v2 num: " << v2.loaded_items.size() << endl;
		//cout << "moved num: " << items1.size() << endl;
		vector<string> items(items1);
		items.insert(items.end(), v2.loaded_items.begin(), v2.loaded_items.end());

		double total_area = 0, total_weight = 0;
		for (string bid : items) {
			total_area += bins.at(bid).get_area();
			total_weight += bins.at(bid).get_weight();
		}

		if (total_area <= v2.get_area() && total_weight <= v2.get_weight()) {
			vector<Bin> Bin_items;
			for (string bid : items)
				Bin_items.push_back(bins.at(bid));
			BPPManager M(v2.get_width(), v2.get_length());
			M.add_bins(Bin_items);

			if (M.checkbpp()) {
				double cost = current_neighbour_cost;
				//移出V1节省的成本
				vector<string> &route = v1.visit_order;
				int route_size = route.size();
				if (route_size == 1)
					cost -= v1.get_flagdown_fare();
				else if (distance(route.begin(), find(route.begin(), route.end(), s.get_id())) == 0)
					cost -= v1.get_distance_fare() * distance_matrix.at(make_pair(route[0], route[1]));
				else if (distance(route.begin(), find(route.begin(), route.end(), s.get_id())) == route_size - 1)
					cost -= v1.get_distance_fare() * distance_matrix.at(make_pair(route[route_size - 2], route[route_size - 1]));
				else {
					int s_idx = distance(route.begin(), find(route.begin(), route.end(), s.get_id()));
					cost -= v1.get_distance_fare() * (distance_matrix.at(make_pair(route[s_idx - 1], route[s_idx])) + \
						distance_matrix.at(make_pair(route[s_idx], route[s_idx + 1])) - \
						distance_matrix.at(make_pair(route[s_idx - 1], route[s_idx + 1])));
				}


				//移入V2增加的成本
				double pre_distance = route_distance(v2.visit_order);
				vector<string> record_order = v2.visit_order;
				double smallest_distance = compute_tsp(v2.visit_order, s.get_id());

				if (smallest_distance != -1) {
					cost += (smallest_distance - pre_distance) * v2.get_distance_fare();
					if (cost < current_neighbour_cost) {
						//to do 机器学习还需要验证装箱
						update_tabusets(v1, v2);
						no_improve_flag = false;
						current_neighbour_cost = cost;
						cout << "New Neighbour Best:" << cost << endl;
						v1.visit_order.erase(find(v1.visit_order.begin(), v1.visit_order.end(), s.get_id()));
						v1.loaded_items = items1_left;
						v1.set_loaded_area(v1.get_loaded_area() - total_area + v2.get_loaded_area());
						v1.set_loaded_weight(v1.get_loaded_weight() - total_weight + v2.get_loaded_weight());
						M.return_seq(v2.loaded_items);
						v2.set_loaded_area(total_area);
						v2.set_loaded_weight(total_weight);
						s.discard(v1.get_id());
						s.pass_vehicles.insert(v2.get_id());
						if (v1.visit_order.empty()) {
							unused_vehicles.insert({ v1.get_id(), v1 });
							used_vehicles.erase(find(used_vehicles.begin(), used_vehicles.end(), v1));
						}
						if (cost < best_known_cost) {
							cout << "New Best Cost:" << cost << endl;
							best_known_cost = cost;
							best_known_sol = save_sol("result\\init_sol.json");
							int num_bins = cal_num_bins();
							/*	if (num_bins != 19280) {
									cout << "*********after********* " << endl;
									cout << "v1 num: " << v1.loaded_items.size() << endl;
									cout << "v2 num: " << v2.loaded_items.size() << endl;
									cout << "v1 num in used_vehicles: " << (*find(used_vehicles.begin(), used_vehicles.end(), v1)).loaded_items.size() << endl;
									cout << "v2 num in used_vehicles: " << (*find(used_vehicles.begin(), used_vehicles.end(), v2)).loaded_items.size() << endl;
									system("pause");
								}*/
							cout << "当前箱子数： " << num_bins << endl;

						}



						//cout << "Current # of bins: " << cal_num_bins() << endl;
						return true;
					}
					else {
						v2.visit_order = record_order;
					}
				}
			}
		}
		tabuset1.insert(make_tuple(v1.get_id(), v2.get_id(), s.get_id()));
		return false;
	}

	bool move2(Station& s1, Station& s2, Vehicle& v1, Vehicle& v2)
	{
		if (tabuset2.find(make_tuple(v1.get_id(), v2.get_id(), s1.get_id(), s2.get_id())) != tabuset2.end())
			return false;
		vector<string> items1;  // v1的s1全移动到V2
		vector<string> minitems1;  // v1至少装的
		vector<string> maxitems2;  // v2至多装的
		vector<string> minitems2;  // v2至少装的
		for (string item : v1.loaded_items) {
			if (s1.get_id() == bins.at(item).get_station())
				items1.push_back(item);
			else
				minitems1.push_back(item);
		}
		vector<string> items2; //v2的s2移动到V1的备选
		for (string item : v2.loaded_items) {
			if (s2.get_id() == bins.at(item).get_station())
				items2.push_back(item);
			else
				minitems2.push_back(item);
		}

		sort(items2.begin(), items2.end(), comp_desca_id);
		minitems2.insert(minitems2.end(), items1.begin(), items1.end());
		maxitems2.insert(maxitems2.end(), items1.begin(), items1.end());
		maxitems2.insert(maxitems2.end(), v2.loaded_items.begin(), v2.loaded_items.end());
		//验证v2能装下最少要装的
		vector<Bin> Bin_minitems2;
		for (string bid : minitems2)
			Bin_minitems2.push_back(bins.at(bid));
		BPPManager M2(v2.get_width(), v2.get_length());
		M2.add_bins(Bin_minitems2);

		if (!M2.checkbpp_sort())
		{
			tabuset2.insert(make_tuple(v1.get_id(), v2.get_id(), s1.get_id(), s2.get_id()));
			return false;
		}

		double total_area1 = 0, total_weight1 = 0, total_area2 = 0, total_weight2 = 0;
		for (string bid : minitems1) {
			total_area1 += bins.at(bid).get_area();
			total_weight1 += bins.at(bid).get_weight();
		}
		for (string bid : maxitems2) {
			total_area2 += bins.at(bid).get_area();
			total_weight2 += bins.at(bid).get_weight();
		}
		size_t n_remove = 1;
		while (n_remove <= items2.size())
		{
			double temp_weight = 0, temp_area = 0;
			for (size_t i = 0; i < n_remove; i++)
			{
				temp_area += bins.at(items2[i]).get_area();
				temp_weight += bins.at(items2[i]).get_weight();
			}

			if ((total_area1 + temp_area) < v1.get_area() && (total_weight1 + temp_weight) < v1.get_weight()
				&& (total_area2 - temp_area) < v2.get_area() && (total_weight2 - temp_weight) < v2.get_weight())
			{
				M2.clear_bin();
				vector<Bin> Bin_tempitems2;
				for (string bid : maxitems2)
				{
					if (find(items2.begin(), items2.begin() + n_remove, bid) == (items2.begin() + n_remove))
						Bin_tempitems2.push_back(bins.at(bid));

				}
				M2.add_bins(Bin_tempitems2);
				if (M2.checkbpp_sort())
				{
					BPPManager M1(v1.get_width(), v1.get_length());
					vector<Bin> Bin_tempitems1;
					for (string bid : minitems1)
						Bin_tempitems1.push_back(bins.at(bid));
					for (size_t j = 0; j < n_remove; j++)
						Bin_tempitems1.push_back(bins.at(items2[j]));
					M1.add_bins(Bin_tempitems1);

					if (M1.checkbpp_sort())
					{

						double cost = current_neighbour_cost;
						//v1 v2成本变化
						vector<string> temp_visit1 = v1.visit_order;
						vector<string> temp_visit2 = v2.visit_order;
						temp_visit1.erase(find(temp_visit1.begin(), temp_visit1.end(), s1.get_id()));
						double pre_distance1 = route_distance(v1.visit_order);
						double smallest_distance1 = compute_tsp(temp_visit1, s2.get_id());
						if (n_remove == items2.size())
							temp_visit2.erase(find(temp_visit2.begin(), temp_visit2.end(), s2.get_id()));

						if (find(temp_visit2.begin(), temp_visit2.end(), s1.get_id()) == temp_visit2.end())
							temp_visit2.push_back(s1.get_id());
						double pre_distance2 = route_distance(v2.visit_order);
						double smallest_distance2 = compute_tsp(temp_visit2);

						if (smallest_distance1 != -1 && smallest_distance2 != -1)
						{
							cost += (smallest_distance1 - pre_distance1) * v1.get_distance_fare();
							cost += (smallest_distance2 - pre_distance2) * v2.get_distance_fare();
							if (cost < current_neighbour_cost)
							{
								update_tabusets(v1, v2);
								no_improve_flag = false;
								current_neighbour_cost = cost;
								cout << "New Neighbour Best:" << cost << endl;
								v1.visit_order = temp_visit1;
								M1.return_seq(v1.loaded_items);

								v1.set_loaded_area(total_area1 + temp_area);
								v1.set_loaded_weight(total_weight1 + temp_weight);
								v2.visit_order = temp_visit2;
								M2.return_seq(v2.loaded_items);
								v2.set_loaded_area(total_area2 - temp_area);
								v2.set_loaded_weight(total_weight2 - temp_weight);
								s1.discard(v1.get_id());
								if (n_remove == items2.size())
									s2.pass_vehicles.erase(v2.get_id());
								s2.pass_vehicles.insert(v1.get_id());
								if (cost < best_known_cost) {
									cout << "New Best Cost:" << cost << endl;
									best_known_cost = cost;
									best_known_sol = save_sol("result\\init_sol.json");
									cout << "best记录解箱子数：" << cal_num_bins() << endl;
								}
								return true;
							}
						}
						else
							break;
					}
				}
			}
			n_remove += 1;
		}
		tabuset2.insert(make_tuple(v1.get_id(), v2.get_id(), s1.get_id(), s2.get_id()));
		return false;
	}

	bool move3(Station& s,
		Vehicle& v1,
		Vehicle& v2) {
		if (tabuset3.find(make_tuple(v1.get_id(), v2.get_id(), s.get_id())) != tabuset3.end())
			return false;
		vector<string> items1;    //v1中站点s的箱子
		vector<string> items1_left;   //v1中不属于站点s的箱子
		for (string item : v1.loaded_items) {
			if (s.get_id() == bins.at(item).get_station())
				items1.push_back(item);
			else
				items1_left.push_back(item);
		}
		double s1_area = 0, s1_weight = 0;
		for (auto bid : items1) {
			s1_area += bins.at(bid).get_area();
			s1_weight += bins.at(bid).get_weight();
		}

		for (string sid2 : v2.visit_order) {
			if (sid2 == s.get_id())
				continue;
			if (stations.at(sid2).get_limit() < v1.get_length())
				continue;
			vector<string> items2;       //v2中站点s2的箱子
			vector<string> items2_left;  //v2中不属于站点s2的箱子
			for (string item : v2.loaded_items) {
				if (sid2 == bins.at(item).get_station())
					items2.push_back(item);
				else
					items2_left.push_back(item);
			}

			double s2_area = 0, s2_weight = 0;
			for (auto bid : items2) {
				s2_area += bins.at(bid).get_area();
				s2_weight += bins.at(bid).get_weight();
			}

			vector<string> total_items1(items1_left);
			total_items1.insert(total_items1.end(), items2.begin(), items2.end());


			double total_area1 = v1.get_loaded_area() - s1_area + s2_area;
			double total_weight1 = v1.get_loaded_weight() - s1_weight + s2_weight;
			if (total_area1 <= v1.get_area() && total_weight1 <= v1.get_weight()) {
				vector<Bin> Bin_total_items1;
				for (string bid : total_items1) {
					Bin_total_items1.push_back(bins.at(bid));
				}
				BPPManager M1(v1.get_width(), v1.get_length());
				M1.add_bins(Bin_total_items1);
				if (M1.checkbpp()) {
					vector<string> total_items2(items2_left);
					total_items2.insert(total_items2.end(), items1.begin(), items1.end());
					double total_area2 = v2.get_loaded_area() - s2_area + s1_area;
					double total_weight2 = v2.get_loaded_weight() - s2_weight + s1_weight;
					if (total_area2 <= v2.get_area() && total_weight2 <= v2.get_weight()) {
						vector<Bin> Bin_total_items2;
						for (string bid : total_items2) {
							Bin_total_items2.push_back(bins.at(bid));
						}
						BPPManager M2(v2.get_width(), v2.get_length());
						M2.add_bins(Bin_total_items2);
						if (M2.checkbpp()) {
							double cost = current_neighbour_cost;
							vector<string> curr_route1 = { sid2 };
							if (v1.visit_order.size() > 1) {
								double pre_distance1 = route_distance(v1.visit_order);
								curr_route1 = v1.visit_order;
								curr_route1.erase(find(curr_route1.begin(), curr_route1.end(), s.get_id()));
								curr_route1.push_back(sid2);
								double smallest_dist1 = compute_tsp(curr_route1);
								if (smallest_dist1 != -1)
									cost += (smallest_dist1 - pre_distance1) * v1.get_distance_fare();
								else
									continue;
							}
							double pre_distance2 = route_distance(v2.visit_order);
							vector<string> curr_route2 = v2.visit_order;
							curr_route2.erase(find(curr_route2.begin(), curr_route2.end(), sid2));
							curr_route2.push_back(s.get_id());
							double smallest_dist2 = compute_tsp(curr_route2);
							if (smallest_dist2 != -1) {
								cost += (smallest_dist2 - pre_distance2) * v2.get_distance_fare();
								if (cost < current_neighbour_cost) {
									update_tabusets(v1, v2);
									no_improve_flag = false;
									current_neighbour_cost = cost;
									cout << "New Neighbour Best:" << cost << endl;
									v1.visit_order = curr_route1;
									v2.visit_order = curr_route2;
									M1.return_seq(v1.loaded_items);
									M2.return_seq(v2.loaded_items);
									v1.set_loaded_area(total_area1);
									v2.set_loaded_area(total_area2);
									v1.set_loaded_weight(total_weight1);
									v2.set_loaded_weight(total_weight2);
									s.pass_vehicles.insert(v2.get_id());
									s.discard(v1.get_id());
									Station &s2 = stations.at(sid2);
									s2.pass_vehicles.insert(v1.get_id());
									s2.discard(sid2);
									if (cost < best_known_cost) {
										cout << "New Best Cost:" << cost << endl;
										best_known_cost = cost;
										best_known_sol = save_sol("result\\init_sol.json");
									}
									return true;
								}
							}
						}
					}
				}

			}

		}
		tabuset3.insert(make_tuple(v1.get_id(), v2.get_id(), s.get_id()));
		return false;
	}

	bool LS1() {
		int count = 0;
		for (auto& v1 : used_vehicles) {
			if (count % print_freq == 0)
				cout << "local search 1 on going: " << count << endl;
			count++;
			for (string sid : v1.visit_order) {
				Station& s = stations.at(sid);
				for (Vehicle& v2 : used_vehicles) {
					if (v2.get_id() != v1.get_id() && v2.get_length() <= s.get_limit())
						if (move1(s, v1, v2))
							return true;
				}
			}
		}
		return false;
	}

	bool LS2()
	{
		int count = 0;
		for (auto& v1 : used_vehicles)
		{
			if (count % print_freq == 0)
				cout << "local search 2 on going: " << count << endl;
			count++;
			for (auto &s1id : v1.visit_order)
			{
				Station& s1 = stations.at(s1id);
				for (auto& v2id : s1.pass_vehicles)
				{
					if (v2id == v1.get_id())
						continue;
					size_t index2 = 0;
					for (; index2 < used_vehicles.size(); index2++)
					{
						if (used_vehicles[index2].get_id() == v2id)
							break;
					}
					Vehicle& v2 = used_vehicles[index2];
					for (auto &s2id : v2.visit_order)
					{
						Station& s2 = stations.at(s2id);
						if (v1.get_length() <= s2.get_limit())
						{
							if (move2(s1, s2, v1, v2))
								return true;
						}
					}
				}
			}
		}
		return false;
	}

	bool LS3() {
		int count = 0;
		for (auto& v1 : used_vehicles) {
			if (count % print_freq == 0)
				cout << "local search 3 on going: " << count << endl;
			count++;
			for (auto &sid : v1.visit_order) {
				Station& s = stations.at(sid);
				for (auto& v2 : used_vehicles) {
					if (v2.get_id() != v1.get_id() && v2.get_length() <= s.get_limit() && v2.visit_order.size() > 1)
						if (move3(s, v1, v2))
							return true;
				}
			}
		}
		return false;
	}


	void Search(bool(*LS)()) {
		while (true) {
			if (LS())
				continue;
			else
				break;
		}
	}

	bool split() {

		double cost_before_split = cal_total_cost();
		int _num = 0;
		for (auto& v : used_vehicles) {
			if (v.occupancy() < 0.85)
				_num++;
		}
		if (_num > 15) {
			sort(used_vehicles.begin(), used_vehicles.end(), cmp_rest_area);
			auto it_min = used_vehicles.begin();
			auto it = used_vehicles.begin();
			while (it != used_vehicles.end()) {
				if ((*it_min).get_area() < (*it).get_area())
				{
					it_min = it;
				}
				it++;
			}
			Vehicle v1(*it_min);
			used_vehicles.erase(it_min);
			used_vehicles.insert(used_vehicles.begin(), v1);

			for (size_t i = 1; i < used_vehicles.size(); ++i) {
				Vehicle& v2 = used_vehicles[i];
				if (v2.get_id() == v1.get_id())
					continue;
				BPPManager M(v2.get_width(), v2.get_length());
				vector<Bin> Bin_v2;
				bid_to_bin(v2.loaded_items, Bin_v2);
				M.add_bins(Bin_v2);
				M.update_backup();
				double total_area = v2.get_loaded_area();
				double total_weight = v2.get_loaded_weight();
				/*		temp_item_sequence = [b.id for b in M.bins]
							temp_visit_order = V2.visit_order
							left = 0
							right = len(V1.loaded_items) - 1
							while left <= right:
						pos = int((left + right) / 2)
							A = total_area + sum(bins[bid].area for bid in V1.loaded_items[:pos + 1])
							W = total_weight + sum(bins[bid].weight for bid in V1.loaded_items[:pos + 1])
							for item in V1.loaded_items[:pos + 1] :
								if stations[bins[item].station].limit < V2.length :
									right = pos - 1
									break*/
				vector<string> temp_item_sequence;
				vector<string> temp_visit_order = v2.visit_order;
				M.return_seq(temp_item_sequence);
				int left = 0;
				int right = v1.loaded_items.size() - 1;
				while (left <= right) {
					int pos = (left + right) / 2;
					double A = total_area, W = total_weight;
					for (size_t j = 0; j < pos + 1; ++j) {
						A += bins.at(v1.loaded_items[j]).get_area();
						W += bins.at(v1.loaded_items[j]).get_weight();
						if (stations.at(bins.at(v1.loaded_items[j]).get_station()).get_limit() < v2.get_length()) {
							right = pos - 1;
							break;
						}
					}
					if (right != pos - 1) {
						if (A <= v2.get_area() && W <= v2.get_weight()) {
							v2.visit_order.insert(v2.visit_order.end(), v1.loaded_items.begin(), v1.loaded_items.begin() + pos + 1);
							double smallest_dist = compute_tsp(v2.visit_order);
							if (smallest_dist == -1) {
								right = pos - 1;
								continue;
							}
							else {
								vector<Bin> Bin_pos;
								bid_to_bin(v1.loaded_items.begin(), v1.loaded_items.begin() + pos + 1, Bin_pos);
								M.add_bins(Bin_pos);
								if (M.checkbpp()) {
									temp_visit_order = v2.visit_order;
									M.return_seq(temp_item_sequence);
									left = pos + 1;
								}
								else {
									right = pos - 1;
								}
							}
						}
						else {
							right = pos - 1;
						}
					}
					else {
						continue;
					}
					M.restore();
				}
				v2.loaded_items = temp_item_sequence;
				v2.visit_order = temp_visit_order;
				for (size_t j = 0; j < left; ++j) {
					v2.set_loaded_area(v2.get_loaded_area() + bins.at(v1.loaded_items[j]).get_area());
					v2.set_loaded_weight(v2.get_loaded_weight() + bins.at(v1.loaded_items[j]).get_weight());
					stations.at(bins.at(v1.loaded_items.at(j)).get_station()).pass_vehicles.insert(v2.get_id());
					stations.at(bins.at(v1.loaded_items.at(j)).get_station()).discard(v1.get_id());
				}

				v1.loaded_items.erase(v1.loaded_items.begin(), v1.loaded_items.begin() + left);
				v1.set_loaded_area(v1.get_loaded_area() - v2.get_loaded_area() + total_area);
				v1.set_loaded_weight(v1.get_loaded_weight() - v2.get_loaded_weight() + total_weight);
				v1.visit_order.clear();
				for (string bid : v1.loaded_items) {
					v1.visit_order.push_back(bins.at(bid).get_station());
					stations.at(bins.at(bid).get_station()).pass_vehicles.insert(v1.get_id());
				}
				compute_tsp(v1.visit_order);
				if (v1.loaded_items.empty()) {
					unused_vehicles.insert({ v1.get_id(), v1 });
					used_vehicles.erase(used_vehicles.begin());
					cout << "Split successfully." << endl;
					break;
				}
			}

		}
		double cost_after_split = cal_total_cost();
		if (cost_after_split < cost_before_split) {
			if (cost_after_split < best_known_cost) {
				best_known_cost = cost_after_split;
				cout << "New Best Cost: " << best_known_cost << endl;
				best_known_sol = save_sol("result\\init_sol.json");
				int num_bins = cal_num_bins();
				cout << "当前箱子数： " << num_bins << endl;
			}
			return true;
		}
		else {
			return false;
		}
	}

}
