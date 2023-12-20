#ifndef _RC_PART_H_
#define _RC_PART_H_
#include<string>
using namespace std;

namespace automat {
	/*Thompson�㷨�������Ĳ�����*/
	struct RCOperand {
		string value;      //����ʽ
		NFAPartInfo info;  //����ʽ����NFA��ʼ��̬��Ϣ

		RCOperand() : value(""), info(NFAPartInfo(-1, -1)) {}
		RCOperand(const string value, const NFAPartInfo info) : value(value), info(info) {}
	};
}

#endif