#pragma once
#ifndef _REGULAR_CALCULATOR_H_
#define _REGULAR_CALCULATOR_H_
#include<iostream>
#include<string>
#include<stack>
#include"NFAPartInfo.h"
#include"RCPart.h"
using namespace std;

namespace automat {
	class NFA;
	class RegularCalculator {
	private:
		static stack<char> postfixStack;    //���ڽ�����ʽת��Ϊ��׺���ʽʱ��ջ
		static stack<RCPart> operateStack;  //��������ʽ����ʱ��ջ

		static int ISP(const char ch);
		static int ICP(const char ch);
		static bool IsOp(const char ch);
		static void AddOperand(const string& value, NFA& nfa);
		static void Add2Operand(const string& value1, const string& value2, NFA& nfa);
		static bool GetOperand(RCPart& operand);
		static bool Get2Operand(RCPart& left, RCPart& right);
		static void DoOperation(const char op, NFA& nfa);
	public:
		static void PostfixPrep(string& str);
		static void Postfix(string& str);
		static void Calculation(string& exp, NFA& nfa);
	};
}

#endif