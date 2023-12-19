#include"RegularCalculator.h"
#include"NFA.h"

namespace automat {
	/*��ʼ�����ڽ�����ʽת��Ϊ��׺���ʽʱ��ջ*/
	stack<char> RegularCalculator::postfixStack = stack<char>();

	/*��ʼ����������ʽ����ʱ��ջ*/
	stack<RCOperand> RegularCalculator::operateStack = stack<RCOperand>();

	/***************************************************************************
	  �������ƣ�ISP
	  ��    �ܣ�����һ���������ջ��������
	  ���������const char ch����������������ֻ����
	  �� �� ֵ��int���������Ӧ��ջ��������
	  ˵    ����������ʽ�������ȼ���| < $ < `��$Ϊ�滻��������㣩
	***************************************************************************/
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

	/***************************************************************************
	  �������ƣ�icp
	  ��    �ܣ�����һ���������ջ��������
	  ���������const char ch����������������ֻ����
	  �� �� ֵ��int���������Ӧ��ջ��������
	  ˵    ����������ʽ�������ȼ���| < $ < `��$Ϊ�滻��������㣩
	***************************************************************************/
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

	/***************************************************************************
	  �������ƣ�IsOp
	  ��    �ܣ��ж�һ���ַ��Ƿ��������
	  ���������const char ch����������������ֻ����
	  �� �� ֵ��bool�����ַ��Ƿ��������
	  ˵    �����������������롢�򡢱հ���������ж�
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
	  �������ƣ�PostfixPrep
	  ��    �ܣ���һ�����ʽ����Ԥ����
	  ���������string& str��������ı��ʽ
	  �� �� ֵ����
	  ˵    ����Ԥ���������ʽ�����������ͼ��Ͻ�����
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
	  �������ƣ�Postfix
	  ��    �ܣ��������ʽΪ��׺���ʽ
	  ���������string& str��������ı��ʽ
	  �� �� ֵ����
	  ˵    ������
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

	/***************************************************************************
	  �������ƣ�AddOperand
	  ��    �ܣ����һ��������
	  ���������const string& value�����ַ�
	            NFA& nfa�����������NFA
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void RegularCalculator::AddOperand(const string& value, NFA& nfa) {
		NFAPartInfo NFAPartInfo = nfa.OperateCreateNew(value);  //���д������ַ����㣬����ʼ��̬��Ϣ
		operateStack.push(RCOperand(value, NFAPartInfo));          //����µĲ�����
	}

	/***************************************************************************
	  �������ƣ�GetOperand
	  ��    �ܣ���ȡһ��������
	  ���������RCOperand& operand��������
	  �� �� ֵ��bool����ȡ�Ƿ�ɹ�
	  ˵    ������
	***************************************************************************/
	bool RegularCalculator::GetOperand(RCOperand& operand) {
		if (operateStack.empty()) {
			cerr << "ջ�ѿգ�" << endl;
			return false;
		}

		operand = operateStack.top();
		operateStack.pop();
		return true;
	}

	/***************************************************************************
	  �������ƣ�Get2Operand
	  ��    �ܣ���ȡ����������
	  ���������RCOperand& left���������
	            RCOperand& right���Ҳ�����
	  �� �� ֵ��bool����ȡ�Ƿ�ɹ�
	  ˵    ������
	***************************************************************************/
	bool RegularCalculator::Get2Operand(RCOperand& left, RCOperand& right) {
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

	/***************************************************************************
	  �������ƣ�DoOperation
	  ��    �ܣ�����һ������
	  ���������const char op�������
				NFA& nfa�����������NFA
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void RegularCalculator::DoOperation(const char op, NFA& nfa) {
		RCOperand left, right, result;  //����������Ҳ����������
		bool flag;  //ȡ�������Ƿ�ɹ��ı�־

		if (op == '`') {  //��Ŀ�հ���ȡһ�������������հ�����
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

		flag = Get2Operand(left, right);  //˫Ŀ�������ȡ����������������Ӧ����
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
	  �������ƣ�Calculation
	  ��    �ܣ���������ʽ����Thompson�㷨����nfa
	  ���������string& exp������ʽ
				NFA& nfa�����������NFA
	  �� �� ֵ����
	  ˵    ������
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
				DoOperation(ch, nfa);  //�������������
				break;
			default:
				operand = "";
				operand += ch;
				AddOperand(operand, nfa);  //����������ַ������
			}

			iter++;
			ch = *iter;
		}
	}
}