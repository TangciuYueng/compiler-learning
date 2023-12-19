#ifndef _DFA_STATE_H_
#define _DFA_STATE_H_
#include<iostream>
#include"DFAProduction.h"
using namespace std;

namespace grammar {
	/*DFA״̬��LR(1)��Ŀ����*/
	struct DFAState {
		int num;  //���
		vector<DFAProduction> productions;  //LR(1)��Ŀ��

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
