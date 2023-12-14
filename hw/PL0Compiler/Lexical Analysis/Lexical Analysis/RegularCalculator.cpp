#include"RegularCalculator.h"
#include"NFA.h"

namespace automat {
	stack<char> RegularCalculator::postfixStack = stack<char>();
	stack<RCPart> RegularCalculator::operateStack = stack<RCPart>();

	int RegularCalculator::ISP(const char ch) {
		int ret = -1;  /*����ֵ������Ϊ������ֵ-1���Է��ַ��Ƿ�*/

		switch (ch) {
		case '#':
			ret = 0;  /*���ʽ��ֹ��*/
			break;
		case '(':
			ret = 1;  /*������*/
			break;
		case '`':
			ret = 7;  /*�հ�*/
			break;
		case '$':
			ret = 5;  /*��*/
			break;
		case '|':
			ret = 3;  /*��*/
			break;
		case ')':
			ret = 8;  /*������*/
			break;
		}

		return ret;
	}

	int RegularCalculator::ICP(const char ch) {
		int ret = -1;  /*����ֵ������Ϊ������ֵ-1���Է��ַ��Ƿ�*/

		switch (ch) {
		case '#':
			ret = 0;  /*���ʽ��ֹ��*/
			break;
		case '(':
			ret = 8;  /*������*/
			break;
		case '`':
			ret = 6;  /*�հ�*/
			break;
		case '$':
			ret = 4;  /*��*/
			break;
		case '|':
			ret = 2;  /*��*/
			break;
		case ')':
			ret = 1;  /*������*/
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
				result += ch;  /*��ӵ�ǰ�ַ�*/
				ch = *iter;    /*������һ���ַ�*/
				iter++;
			}

			else {
				top = postfixStack.top();  /*ȡջ�������*/

				if (ISP(top) < ICP(ch)) {  /*����ǰ�ַ���ջ�⣩���ȼ�����ջ���������ջ�ڣ������ȼ����򽫵�ǰ�ַ�ѹ��ջ��*/
					postfixStack.push(ch);
					ch = *iter;  /*������һ���ַ�*/
					iter++;
				}

				else if (ISP(top) > ICP(ch)) {  /*����ǰ�ַ���ջ�⣩���ȼ�С��ջ���������ջ�ڣ������ȼ����򵯳�ջ��Ԫ�أ�����ӿո��ջ��Ԫ��*/
					pop = postfixStack.top();
					postfixStack.pop();
					result += pop;
				}

				else {  /*���������ȼ���ȣ��򵯳�ջ��Ԫ��*/
					pop = postfixStack.top();
					postfixStack.pop();

					if (pop == '(') {  /*��ջ��Ԫ���������ţ�ֱ�Ӷ�����һ���ַ�*/
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
			cerr << "ջ�ѿգ�" << endl;
			return false;
		}

		operand = operateStack.top();
		operateStack.pop();
		return true;
	}

	bool RegularCalculator::Get2Operand(RCPart& left, RCPart& right) {
		if (operateStack.empty()) {
			cerr << "ȱ���Ҳ�������" << endl;
			return false;
		}
		right = operateStack.top();
		operateStack.pop();

		if (operateStack.empty()) {
			cerr << "ȱ�����������" << endl;
			return false;
		}
		left = operateStack.top();
		operateStack.pop();

		return true;
	}

	void RegularCalculator::DoOperation(const char op, NFA& nfa) {
		RCPart left, right, result;  /*����������Ҳ����������*/
		bool flag;  /*ȡ�������Ƿ�ɹ��ı�־*/

		if (op == '`') {  /*��Ŀ�հ���ȡһ�������������հ�����*/
			flag = GetOperand(left);
			if (flag) {
				result.info = nfa.OperateClosure(left.info);
				result.value = '(' + left.value + ")`";

				/*cout << "�հ����㣺" << endl
					<< "Դ��������" << endl
					<< left.value << endl
					<< "�����" << result.value << endl << endl;*/
				operateStack.push(result);
				return;
			}

			else {
				return;
			}
		}

		flag = Get2Operand(left, right);  /*˫Ŀ�������ȡ����������������Ӧ����*/
		if (flag) {
			switch (op) {
			case '|':
				result.value = left.value + '|' + right.value;
				result.info = nfa.OperateOr(left.info, right.info);

				/*cout << "�����㣺" << endl
					<< "Դ��������" << endl
					<< left.value << endl
					<< right.value << endl
					<< "�����" << result.value << endl << endl;*/
				operateStack.push(result);
				break;
			case '$':
				result.value = left.value + right.value;
				result.info = nfa.OperateAnd(left.info, right.info);

				/*cout << "�����㣺" << endl
					<< "Դ��������" << endl
					<< left.value << endl
					<< right.value << endl
					<< "�����" << result.value << endl << endl;*/
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