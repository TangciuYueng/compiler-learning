#ifndef _NFA_TRANS_H_
#define _NFA_TRANS_H_
#include<string>
using namespace std;

namespace automat {
	/*NFA转换信息*/
	struct NFATrans {
		int start;    //始态
		int end;      //终态
		string recv;  //接收的字符

		NFATrans(const int start, const int end, const string recv) : start(start), end(end), recv(recv) {}
	};
}

#endif