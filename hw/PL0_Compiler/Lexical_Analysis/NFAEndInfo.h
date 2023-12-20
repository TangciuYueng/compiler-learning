#ifndef _NFA_END_INFO_H_
#define _NFA_END_INFO_H_
#include<string>
using namespace std;

namespace automat {
	/*NFA终态信息*/
	struct NFAEndInfo {
		string regular;  //识别的正规式
		int priority;    //优先级

		NFAEndInfo(const string regular = "", const int priority = INT_MAX) : regular(regular), priority(priority) {}
	};
}

#endif