#ifndef _NORM_STATE_H_
#define _NORM_STATE_H_
#include<set>
using namespace std;

namespace automat {
	/*DFA状态*/
	struct DFAState {
		set<int> state;  //包含的原NFA状态集合
		int num;         //在DFA中的编号

		DFAState(set<int>& state, const int num) : state(state), num(num) {}
	};
}

#endif