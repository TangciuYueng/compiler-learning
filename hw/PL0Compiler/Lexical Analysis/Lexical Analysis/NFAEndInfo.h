#ifndef _NFA_END_INFO_H_
#define _NFA_END_INFO_H_
#include<string>
using namespace std;

namespace automat {
	struct NFAEndInfo {
		string regular;
		int priority;

		NFAEndInfo(const string regular = "", const int priority = INT_MAX) : regular(regular), priority(priority) {}
	};
}

#endif