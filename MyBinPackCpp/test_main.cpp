#include <iostream>
#include <vector>
#include <algorithm>
#include "util.h"
#include <string>
#include <unordered_set>
using namespace std;



int main() {
	unordered_set<int> a = { 3,3,4,5,6 };
	auto it = a.begin();

	while (it != a.end()) {
		if (*it < 5) {
			it = a.erase(it);
		}
		else {
			it++;

		}
	}


	for (auto item : a) {
		cout << item << endl;
	}
	getchar();
	return 0;
}