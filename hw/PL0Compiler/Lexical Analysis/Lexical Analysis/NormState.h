#ifndef _NORM_STATE_H_
#define _NORM_STATE_H_
#include<set>
using namespace std;

namespace automat {
	struct NormState {
		set<int> state;
		int num;

		NormState(set<int>& state, const int num) : state(state), num(num) {}
	};
}

#endif