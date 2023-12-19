#pragma once
#ifndef _NFA_H_
#define _NFA_H_
#include<string>
#include<vector>
#include<map>
#include<climits>
#include"Auto.h"
#include"NFAPartInfo.h"
#include"NFAEndInfo.h"
#include"NFATrans.h"
using namespace std;

namespace automat {
	/*��ȷ�������Զ���*/
	class NFA : public Auto {
	private:
		int start;  //��ʼ̬
		int end;    //��̬
		map<int, NFAEndInfo> endInfoMap;  //��̬��Ϣ��

	public:
		static int stateCount;  //����NFA״̬����
		static vector<NFATrans> totalTransList;  //����NFAת����Ϣ��

		NFA() : start(0), end(1) {}  //ʹ��Thompson�㷨������NFA����һ����̬����̬
		NFAPartInfo OperateCreateNew(const string newContent);
		NFAPartInfo OperateOr(const NFAPartInfo part1, const NFAPartInfo part2);
		NFAPartInfo OperateAnd(const NFAPartInfo part1, const NFAPartInfo part2);
		NFAPartInfo OperateClosure(const NFAPartInfo part);
		NFAPartInfo GetInfo()const;
		const GraphList<int, string>* GetTransList()const;
		const set<int>* GetEndList()const;
		const map<int, NFAEndInfo>* GetEndInfoMap()const;
		void MakeTotalNFA(const int newStart, set<int>& newEndList, map<int, NFAEndInfo>& newEndInfoMap);
		void Show()const;
	};
}

#endif