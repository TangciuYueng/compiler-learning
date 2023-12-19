#ifndef _ELEMENT_H_
#define _ELEMENT_H_
#include<string>
using namespace std;

namespace grammar {
	/*语法元素*/
	struct Element {
		string word;      //词
		bool ifTerminal;  //是否为终结符

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
