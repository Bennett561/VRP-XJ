﻿#pragma once
#include "entity\Vehicle.h"
#include "entity\Bin.h"
#include "entity\Station.h"
#include "maxrec\MaxRectsBinPack.h"
#include "maxrec\Rect.h"
#include "custom_hash.h"
#include <iostream>
#include <map>
#include "util.h" //工具函数

using namespace std;

class BPPManager
{
public:
	BPPManager(double vw, double vl) :veh_w(vw), veh_l(vl) {
		this->heuristic = rbp::MaxRectsBinPack::RectBottomLeftRule;
	}
	bool checkbpp();
	bool checkbpp_sort();
	void clear_bin();
	void add_bins(vector<Bin>& bins);
	void return_seq(vector<string>& loaded_items);
	void update_backup();
	void restore();
private:
	vector<Bin> own_bins;
	vector<Bin> backup_bins;
	double veh_w;
	double veh_l;
	rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic;
};

void BPPManager::add_bins(vector<Bin>& bins) { 
	if(bins.empty())
		this->own_bins = bins; 
	else {
		own_bins.insert(own_bins.end(), bins.begin(), bins.end());
	}
}

void BPPManager::update_backup() {
	backup_bins = own_bins;
}
void BPPManager::restore() {
	own_bins = backup_bins;
}

bool BPPManager::checkbpp()
{
	rbp::MaxRectsBinPack MaxRec;
	MaxRec.Init(veh_w, veh_l);
	for (unsigned i = 0; i < own_bins.size(); i++)
	{
		rbp::Rect packedRect = MaxRec.Insert(own_bins[i].get_width(), own_bins[i].get_length(), this->heuristic);
		if (packedRect.height == 0)
			return 0;
	}
	return 1;

}

void BPPManager::clear_bin()
{
	this->own_bins.clear();
}


void BPPManager::return_seq(vector<string>& loaded_items)
{
	loaded_items.clear();
	for (unsigned i = 0; i < own_bins.size(); i++)
	{
		loaded_items.push_back(own_bins[i].get_id());
	}
}

bool calcor(vector<Bin>& packed_bins, Vehicle veh)//理论上bin已经排序完成，不须调用calcor_sort，只用这个就行
{
	rbp::MaxRectsBinPack MaxRec;
	rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = rbp::MaxRectsBinPack::RectBottomLeftRule;
	MaxRec.Init(veh.get_width(), veh.get_length());
	for (unsigned i = 0; i < packed_bins.size(); i++)
	{
		rbp::Rect packedRect = MaxRec.Insert(packed_bins[i].get_width(), packed_bins[i].get_length(), heuristic);
		if (packedRect.height == 0)
			return 0;
		packed_bins[i].set_x(packedRect.x);
		packed_bins[i].set_y(packedRect.y);
		packed_bins[i].set_real_width(packedRect.width);
		packed_bins[i].set_real_length(packedRect.height);
	}
	return 1;
}

bool BPPManager::checkbpp_sort()
{
	if (this->checkbpp())//按放入顺序
		return 1;
	sort(own_bins.begin(), own_bins.end(), fun_comp[0]);//面积降序
	if (this->checkbpp())
		return 1;
	for (unsigned i = 0; i < own_bins.size() - 1; i++)
	{
		if (own_bins[i].get_area() > 1.5)
		{
			swap(own_bins[i], own_bins[i + 1]);
			if (this->checkbpp())
				return 1;
			else
				swap(own_bins[i], own_bins[i + 1]);
		}
		else
			break;
	}
	for (int i = 1; i < 6; i++)
	{
		sort(own_bins.begin(), own_bins.end(), fun_comp[i]);
		if (this->checkbpp())
			return 1;
	}
	return 0;
}

bool calcor_sort(vector<Bin>& packed_bins, Vehicle veh)
{
	if (calcor(packed_bins, veh))
		return 1;
	sort(packed_bins.begin(), packed_bins.end(), fun_comp[0]);//面积降序
	if (calcor(packed_bins, veh))
		return 1;
	for (unsigned i = 0; i < packed_bins.size() - 1; i++)
	{
		if (packed_bins[i].get_area() > 1.5)
		{
			swap(packed_bins[i], packed_bins[i + 1]);
			if (calcor(packed_bins, veh))
				return 1;
			else
				swap(packed_bins[i], packed_bins[i + 1]);
		}
		else
			break;
	}
	for (int i = 1; i < 6; i++)
	{
		sort(packed_bins.begin(), packed_bins.end(), fun_comp[i]);
		if (calcor(packed_bins, veh))
			return 1;
	}
	return 0;

}