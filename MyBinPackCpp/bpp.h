#pragma once
#include "entity\Vehicle.h"
#include "entity\Bin.h"
#include "entity\Station.h"
#include "maxrec\MaxRectsBinPack.h"
#include "maxrec\Rect.h"
#include <iostream>
#include <map>
#include "util.h" //¹¤¾ßº¯Êý

using namespace std;

class BPPManager
{
public:
	BPPManager(double vw, double vl) { this->veh_w = vw;this->veh_l = vl; }
	bool checkbpp();
	void add_bins(vector<Bin>& bins) { this->own_bins = bins; }
	vector<string> return_seq();
private:
	vector<Bin> own_bins;
	double veh_w;
	double veh_l;
};



bool BPPManager::checkbpp()
{
	rbp::MaxRectsBinPack MaxRec;
	rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = rbp::MaxRectsBinPack::RectBottomLeftRule;
	MaxRec.Init(veh_w, veh_l);
	for (unsigned i = 0;i < own_bins.size();i++)
	{
		rbp::Rect packedRect = MaxRec.Insert(own_bins[i].get_width(), own_bins[i].get_length(), heuristic);
		if (packedRect.height == 0)
			return 0;
	}
	return 1;

}

vector<string> BPPManager::return_seq()
{
	vector<string> seq;
	for (unsigned i = 0;i < own_bins.size();i++)
	{
		seq.push_back(own_bins[i].get_id());
	}
	return seq;
}

bool calcor(vector<Bin>& bins, Vehicle veh)
{
	rbp::MaxRectsBinPack MaxRec;
	rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = rbp::MaxRectsBinPack::RectBestShortSideFit;
	MaxRec.Init(veh.get_width(), veh.get_length());
	for (unsigned i = 0;i < bins.size();i++)
	{
		rbp::Rect packedRect = MaxRec.Insert(bins[i].get_width(), bins[i].get_length(), heuristic);
		//if (packedRect.height == 0)
			//return 0;
		bins[i].set_x(packedRect.x);
		bins[i].set_y(packedRect.y);
		bins[i].set_real_width(packedRect.width);
		bins[i].set_real_length(packedRect.height);
	}
	return 1;

}