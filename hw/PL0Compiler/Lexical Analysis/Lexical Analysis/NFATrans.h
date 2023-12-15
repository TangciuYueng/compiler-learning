#ifndef _NFA_TRANS_H_
#define _NFA_TRANS_H_
#include<string>
using namespace std;

namespace automat {
	struct NFATrans {
		int start;
		int end;
		string recv;

		NFATrans(const int start, const int end, const string recv) : start(start), end(end), recv(recv) {}
	};
}

#endif