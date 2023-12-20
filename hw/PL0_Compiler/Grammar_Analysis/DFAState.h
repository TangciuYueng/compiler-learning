#ifndef _DFA_STATE_H_
#define _DFA_STATE_H_
#include<iostream>
#include"DFAProduction.h"
using namespace std;

namespace grammar {
	/*DFA状态（LR(1)项目集）*/
	struct DFAState {
		int num;  //编号
		vector<DFAProduction> productions;  //LR(1)项目集

		DFAState() : num(-1), productions(vector<DFAProduction>()) {}
		DFAState(const int num, vector<DFAProduction>& productions) : num(num), productions(productions) {}

		friend ostream& operator<<(ostream& out, const DFAState& state) {
			for (DFAProduction production : state.productions) {
				out << production << endl;
			}

			return out;
		}

		friend bool operator==(const DFAState& left, const DFAState& right) {
			return left.productions == right.productions;
		}

		friend bool operator<(const DFAState& left, const DFAState& right) {
			return left.num < right.num;
		}
	};
}

#endif
