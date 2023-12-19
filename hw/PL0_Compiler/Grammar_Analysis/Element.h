#ifndef _ELEMENT_H_
#define _ELEMENT_H_
#include<string>
using namespace std;

namespace grammar {
	/*�﷨Ԫ��*/
	struct Element {
		string word;      //��
		bool ifTerminal;  //�Ƿ�Ϊ�ս��

		Element(const string word, const bool ifTerminal) : word(word), ifTerminal(ifTerminal) {}

		friend bool operator==(const Element& left, const Element& right) {
			return left.word == right.word && left.ifTerminal == right.ifTerminal;
		}

		friend bool operator!=(const Element& left, const Element& right) {
			return left.word != right.word || left.ifTerminal != right.ifTerminal;
		}

		friend bool operator<(const Element& left, const Element& right) {
			return left.word < right.word;
		}

		friend bool operator>(const Element& left, const Element& right) {
			return left.word > right.word;
		}

		friend bool operator<=(const Element& left, const Element& right) {
			return left.word <= right.word;
		}

		friend bool operator>=(const Element& left, const Element& right) {
			return left.word >= right.word;
		}
	};
}

#endif
