#ifndef _NFA_TRANS_H_
#define _NFA_TRANS_H_
#include<string>
using namespace std;

namespace automat {
	/*NFAת����Ϣ*/
	struct NFATrans {
		int start;    //ʼ̬
		int end;      //��̬
		string recv;  //���յ��ַ�

		NFATrans(const int start, const int end, const string recv) : start(start), end(end), recv(recv) {}
	};
}

#endif