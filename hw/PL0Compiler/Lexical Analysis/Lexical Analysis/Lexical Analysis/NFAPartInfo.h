#ifndef _NFA_PART_INFO_H_
#define _NFA_PART_INFO_H_

namespace automat {
	struct NFAPartInfo {
		int start;
		int end;

		NFAPartInfo(const int start, const int end) : start(start), end(end) {}
	};
}

#endif