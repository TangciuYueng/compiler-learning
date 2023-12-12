#ifndef _RC_OPERATE_H_
#define _RC_OPERATE_H_

#define RC_CREATE_NEW 0
#define RC_OR         1
#define RC_AND        2
#define RC_CLOSURE    3

struct RCOperate {
	int op;
	int start;
	int end;

	RCOperate(const int op, const int start, const int end) : op(op), start(start), end(end) {}
};

#endif