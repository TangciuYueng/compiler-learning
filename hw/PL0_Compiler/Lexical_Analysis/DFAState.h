#ifndef _NORM_STATE_H_
#define _NORM_STATE_H_
#include<set>
using namespace std;

namespace automat {
	/*DFA״̬*/
	struct DFAState {
		set<int> state;  //������ԭNFA״̬����
		int num;         //��DFA�еı��

		DFAState(set<int>& state, const int num) : state(state), num(num) {}
	};
}

#endif