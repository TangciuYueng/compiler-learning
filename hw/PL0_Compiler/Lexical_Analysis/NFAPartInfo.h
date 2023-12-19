#ifndef _NFA_PART_INFO_H_
#define _NFA_PART_INFO_H_

namespace automat {
	/*NFA部分信息*/
	struct NFAPartInfo {
		int start;  //始态
		int end;    //终态

		NFAPartInfo(const int start, const int end) : start(start), end(end) {}
	};
}

#endif