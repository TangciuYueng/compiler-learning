#include"RegularCalculator.h"
#include"NFA.h"

namespace automat {
	stack<char> RegularCalculator::postfixStack = stack<char>();
	stack<RCPart> RegularCalculator::operateStack = stack<RCPart>();

	int RegularCalculator::ISP(const char ch) {
		int ret = -1;  /*返回值，先置为不可能值-1，以防字符非法*/

		switch (ch) {
		case '#':
			ret = 0;  /*表达式终止符*/
			break;
		case '(':
			ret = 1;  /*左括号*/
			break;
		case '`':
			ret = 7;  /*闭包*/
			break;
		case '$':
			ret = 5;  /*与*/
			break;
		case '|':
			ret = 3;  /*或*/
			break;
		case ')':
			ret = 8;  /*右括号*/
			break;
		}

		return ret;
	}

	int RegularCalculator::ICP(const char ch) {
		int ret = -1;  /*返回值，先置为不可能值-1，以防字符非法*/

		switch (ch) {
		case '#':
			ret = 0;  /*表达式终止符*/
			break;
		case '(':
			ret = 8;  /*左括号*/
			break;
		case '`':
			ret = 6;  /*闭包*/
			break;
		case '$':
			ret = 4;  /*与*/
			break;
		case '|':
			ret = 2;  /*或*/
			break;
		case ')':
			ret = 1;  /*右括号*/
			break;
		}

		return ret;
	}

	bool RegularCalculator::IsOp(const char ch)
	{
		switch (ch) {
		case '|':
		case '$':
		case '`':
			return true;
			break;
		default:
			return false;
		}
	}

	void RegularCalculator::PostfixPrep(string& str) {
		if (str.empty())
			return;

		string result;
		string::iterator iter = str.begin();

		while (iter != str.end()) {
			result += *iter;

			if (*iter != '(' && iter + 1 != str.end() && !IsOp(*(iter + 1)) && *(iter + 1) != ')') {
				if (*(iter + 1) == '(' && *iter != '(')
					result += '$';
				else if (*iter != '|' && *iter != '$') {
					result += '$';
				}
			}


			iter++;
		}

		result += '#';
		str = result;
	}

	void RegularCalculator::Postfix(string& src) {
		string::iterator iter = src.begin();
		string result;

		char ch = '#', top, pop;

		postfixStack.push(ch);
		ch = *iter;
		iter++;

		while (!postfixStack.empty()) {
			if (iter != src.end() && !IsOp(ch) && ch != '(' && ch != ')') {
				result += ch;  /*添加当前字符*/
				ch = *iter;    /*读入下一个字符*/
				iter++;
			}

			else {
				top = postfixStack.top();  /*取栈顶运算符*/

				if (ISP(top) < ICP(ch)) {  /*若当前字符（栈外）优先级大于栈顶运算符（栈内）的优先级，则将当前字符压入栈中*/
					postfixStack.push(ch);
					ch = *iter;  /*读入下一个字符*/
					iter++;
				}

				else if (ISP(top) > ICP(ch)) {  /*若当前字符（栈外）优先级小于栈顶运算符（栈内）的优先级，则弹出栈顶元素，并添加空格和栈顶元素*/
					pop = postfixStack.top();
					postfixStack.pop();
					result += pop;
				}

				else {  /*若两者优先级相等，则弹出栈顶元素*/
					pop = postfixStack.top();
					postfixStack.pop();

					if (pop == '(') {  /*若栈顶元素是左括号，直接读入下一个字符*/
						ch = *iter;
						iter++;
					}
				}
			}
		}

		src = result;
	}

	void RegularCalculator::AddOperand(const string& value, NFA& nfa) {
		NFAPartInfo NFAPartInfo = nfa.OperateCreateNew(value);
		operateStack.push(RCPart(value, NFAPartInfo));
	}

	void RegularCalculator::Add2Operand(const string& value1, const string& value2, NFA& nfa) {
		NFAPartInfo NFAPartInfo1 = nfa.OperateCreateNew(value1);
		NFAPartInfo NFAPartInfo2 = nfa.OperateCreateNew(value2);
		operateStack.push(RCPart(value1, NFAPartInfo1));
		operateStack.push(RCPart(value2, NFAPartInfo2));
	}

	bool RegularCalculator::GetOperand(RCPart& operand) {
		if (operateStack.empty()) {
			cerr << "栈已空！" << endl;
			return false;
		}

		operand = operateStack.top();
		operateStack.pop();
		return true;
	}

	bool RegularCalculator::Get2Operand(RCPart& left, RCPart& right) {
		if (operateStack.empty()) {
			cerr << "缺少右操作数！" << endl;
			return false;
		}
		right = operateStack.top();
		operateStack.pop();

		if (operateStack.empty()) {
			cerr << "缺少左操作数！" << endl;
			return false;
		}
		left = operateStack.top();
		operateStack.pop();

		return true;
	}

	void RegularCalculator::DoOperation(const char op, NFA& nfa) {
		RCPart left, right, result;  /*左操作数、右操作数、结果*/
		bool flag;  /*取操作数是否成功的标志*/

		if (op == '`') {  /*单目闭包，取一个操作数并做闭包运算*/
			flag = GetOperand(left);
			if (flag) {
				result.info = nfa.OperateClosure(left.info);
				result.value = '(' + left.value + ")`";

				/*cout << "闭包运算：" << endl
					<< "源操作数：" << endl
					<< left.value << endl
					<< "结果：" << result.value << endl << endl;*/
				operateStack.push(result);
				return;
			}

			else {
				return;
			}
		}

		flag = Get2Operand(left, right);  /*双目运算符，取两个操作数并做对应运算*/
		if (flag) {
			switch (op) {
			case '|':
				result.value = left.value + '|' + right.value;
				result.info = nfa.OperateOr(left.info, right.info);

				/*cout << "或运算：" << endl
					<< "源操作数：" << endl
					<< left.value << endl
					<< right.value << endl
					<< "结果：" << result.value << endl << endl;*/
				operateStack.push(result);
				break;
			case '$':
				result.value = left.value + right.value;
				result.info = nfa.OperateAnd(left.info, right.info);

				/*cout << "与运算：" << endl
					<< "源操作数：" << endl
					<< left.value << endl
					<< right.value << endl
					<< "结果：" << result.value << endl << endl;*/
				operateStack.push(result);
				break;
			}
		}

		else
			return;
	}

	void RegularCalculator::Calculation(string& exp, NFA& nfa) {
		char ch;
		string operand;
		string::iterator iter = exp.begin();
		ch = *iter;

		while (ch != '#') {
			switch (ch) {
			case '|':
			case '$':
			case '`':
				DoOperation(ch, nfa);
				break;
			default:
				operand = "";
				operand += ch;
				AddOperand(operand, nfa);
			}

			iter++;
			ch = *iter;
		}
	}
}