#ifndef _TRANS_H_
#define _TRANS_H_

namespace grammar {
	/*动作种类*/
	enum TransType {
		ACTION_SHIFT,
		ACTION_REDUCE,
		ACTION_ACC,
		GOTO
	};

	/*LR(1)分析表动作*/
	struct Trans {
		TransType type;  //动作种类
		int dest;        //目标状态/归约产生式编号

		Trans(const TransType type, const int dest = -1) : type(type), dest(dest) {}

		string Format()const {
			switch (this->type) {
			case TransType::ACTION_SHIFT:
				return string("s") += to_string(dest);
				break;
			case TransType::ACTION_REDUCE:
				return string("r") += to_string(dest);
				break;
			case TransType::ACTION_ACC:
				return string("acc");
				break;
			case TransType::GOTO:
				return string("") += to_string(dest);
				break;
			default:
				return "";
			}
		}

		friend bool operator<(const Trans& left, const Trans& right) {
			if (left.type != right.type) {
				return left.type < right.type;
			}

			return left.dest < right.dest;
		}
	};
}

#endif
