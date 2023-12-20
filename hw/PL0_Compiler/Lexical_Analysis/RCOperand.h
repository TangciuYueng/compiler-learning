#ifndef _RC_PART_H_
#define _RC_PART_H_
#include<string>
using namespace std;

namespace automat {
	/*Thompson算法计算器的操作数*/
	struct RCOperand {
		string value;      //正规式
		NFAPartInfo info;  //正规式生成NFA的始终态信息

		RCOperand() : value(""), info(NFAPartInfo(-1, -1)) {}
		RCOperand(const string value, const NFAPartInfo info) : value(value), info(info) {}
	};
}

#endif