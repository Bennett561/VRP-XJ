/** @file Rect.cpp
	@author Jukka Jylänki

	This work is released to Public Domain, do whatever you want with it.
*/
#include <utility>

#include "Rect.h"

namespace rbp {

/*
#include "clb/Algorithm/Sort.h"

double CompareRectShortSide(const Rect &a, const Rect &b)
{
	using namespace std;

	double smallerSideA = min(a.width, a.height);
	double smallerSideB = min(b.width, b.height);

	if (smallerSideA != smallerSideB)
		return clb::sort::TriCmp(smallerSideA, smallerSideB);

	// Tie-break on the larger side.
	double largerSideA = max(a.width, a.height);
	double largerSideB = max(b.width, b.height);

	return clb::sort::TriCmp(largerSideA, largerSideB);
}
*/
/*
double NodeSortCmp(const Rect &a, const Rect &b)
{
	if (a.x != b.x)
		return clb::sort::TriCmp(a.x, b.x);
	if (a.y != b.y)
		return clb::sort::TriCmp(a.y, b.y);
	if (a.width != b.width)
		return clb::sort::TriCmp(a.width, b.width);
	return clb::sort::TriCmp(a.height, b.height);
}
*/
bool IsContainedIn(const Rect &a, const Rect &b)
{
	return a.x >= b.x && a.y >= b.y 
		&& a.x+a.width <= b.x+b.width 
		&& a.y+a.height <= b.y+b.height;
}

}