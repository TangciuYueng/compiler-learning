#ifndef _NFA_PART_INFO_H_
#define _NFA_PART_INFO_H_

namespace automat {
	/*NFA������Ϣ*/
	struct NFAPartInfo {
		int start;  //ʼ̬
		int end;    //��̬

		NFAPartInfo(const int start, const int end) : start(start), end(end) {}
	};
}

#endif