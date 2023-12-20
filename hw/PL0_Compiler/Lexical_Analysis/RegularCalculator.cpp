#include"RegularCalculator.h"
#include"NFA.h"

namespace automat {
	/*初始化用于将正规式转换为后缀表达式时的栈*/
	stack<char> RegularCalculator::postfixStack = stack<char>();

	/*初始化用于正规式运算时的栈*/
	stack<RCOperand> RegularCalculator::operateStack = stack<RCOperand>();

	/***************************************************************************
	  函数名称：ISP
	  功    能：返回一个运算符的栈内优先数
	  输入参数：const char ch：待处理的运算符（只读）
	  返 回 值：int：运算符对应的栈内优先数
	  说    明：设正规式运算优先级：| < $ < `（$为替换后的与运算）
	***************************************************************************/
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

	/***************************************************************************
	  函数名称：icp
	  功    能：返回一个运算符的栈外优先数
	  输入参数：const char ch：待处理的运算符（只读）
	  返 回 值：int：运算符对应的栈外优先数
	  说    明：设正规式运算优先级：| < $ < `（$为替换后的与运算）
	***************************************************************************/
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

	/***************************************************************************
	  函数名称：IsOp
	  功    能：判断一个字符是否是运算符
	  输入参数：const char ch：待处理的运算符（只读）
	  返 回 值：bool：该字符是否是运算符
	  说    明：本函数仅限于与、或、闭包运算符的判断
	***************************************************************************/
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

	/***************************************************************************
	  函数名称：PostfixPrep
	  功    能：对一个表达式进行预处理
	  输入参数：string& str：待处理的表达式
	  返 回 值：无
	  说    明：预处理包括显式添加与运算符和加上结束符
	***************************************************************************/
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

	/***************************************************************************
	  函数名称：Postfix
	  功    能：调整表达式为后缀表达式
	  输入参数：string& str：待处理的表达式
	  返 回 值：无
	  说    明：无
	***************************************************************************/
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

	/***************************************************************************
	  函数名称：AddOperand
	  功    能：添加一个操作数
	  输入参数：const string& value：新字符
	            NFA& nfa：进行运算的NFA
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void RegularCalculator::AddOperand(const string& value, NFA& nfa) {
		NFAPartInfo NFAPartInfo = nfa.OperateCreateNew(value);  //进行创建新字符运算，返回始终态信息
		operateStack.push(RCOperand(value, NFAPartInfo));          //添加新的操作数
	}

	/***************************************************************************
	  函数名称：GetOperand
	  功    能：获取一个操作数
	  输入参数：RCOperand& operand：操作数
	  返 回 值：bool：获取是否成功
	  说    明：无
	***************************************************************************/
	bool RegularCalculator::GetOperand(RCOperand& operand) {
		if (operateStack.empty()) {
			cerr << "栈已空！" << endl;
			return false;
		}

		operand = operateStack.top();
		operateStack.pop();
		return true;
	}

	/***************************************************************************
	  函数名称：Get2Operand
	  功    能：获取两个操作数
	  输入参数：RCOperand& left：左操作数
	            RCOperand& right：右操作数
	  返 回 值：bool：获取是否成功
	  说    明：无
	***************************************************************************/
	bool RegularCalculator::Get2Operand(RCOperand& left, RCOperand& right) {
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

	/***************************************************************************
	  函数名称：DoOperation
	  功    能：进行一次运算
	  输入参数：const char op：运算符
				NFA& nfa：进行运算的NFA
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void RegularCalculator::DoOperation(const char op, NFA& nfa) {
		RCOperand left, right, result;  //左操作数、右操作数、结果
		bool flag;  //取操作数是否成功的标志

		if (op == '`') {  //单目闭包，取一个操作数并做闭包运算
			flag = GetOperand(left);
			if (flag) {
				result.info = nfa.OperateClosure(left.info);
				result.value = '(' + left.value + ")`";
				operateStack.push(result);
				return;
			}

			else {
				return;
			}
		}

		flag = Get2Operand(left, right);  //双目运算符，取两个操作数并做对应运算
		if (flag) {
			switch (op) {
			case '|':
				result.value = left.value + '|' + right.value;
				result.info = nfa.OperateOr(left.info, right.info);
				operateStack.push(result);
				break;
			case '$':
				result.value = left.value + right.value;
				result.info = nfa.OperateAnd(left.info, right.info);
				operateStack.push(result);
				break;
			}
		}

		else
			return;
	}

	/***************************************************************************
	  函数名称：Calculation
	  功    能：根据正规式，用Thompson算法构建nfa
	  输入参数：string& exp：正规式
				NFA& nfa：进行运算的NFA
	  返 回 值：无
	  说    明：无
	***************************************************************************/
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
				DoOperation(ch, nfa);  //有运算符就运算
				break;
			default:
				operand = "";
				operand += ch;
				AddOperand(operand, nfa);  //否则就是新字符，添加
			}

			iter++;
			ch = *iter;
		}
	}
}