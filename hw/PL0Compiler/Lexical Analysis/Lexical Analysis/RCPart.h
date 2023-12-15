#ifndef _RC_PART_H_
#define _RC_PART_H_
#include<string>
using namespace std;

namespace automat {
	struct RCPart {
		string value;
		NFAPartInfo info;

		RCPart() : value(""), info(NFAPartInfo(-1, -1)) {}
		RCPart(const string value, const NFAPartInfo info) : value(value), info(info) {}
	};
}

#endif