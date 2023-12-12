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
	class NFA : public Auto {
	private:
		int start;
		int end;    //使用Thompson算法产生的NFA仅有一个初态和终态
		map<int, NFAEndInfo> endInfoMap;  //终态信息表

	public:
		static int stateCount;  //所有NFA状态总数
		static vector<NFATrans> totalTransList;  //所有NFA转换信息表
		static void ShowTotalTransList();        //显示所有NFA转换信息表

		NFA() : start(0), end(1) {}
		NFAPartInfo OperateCreateNew(const string newContent);
		NFAPartInfo OperateOr(const NFAPartInfo part1, const NFAPartInfo part2);
		NFAPartInfo OperateAnd(const NFAPartInfo part1, const NFAPartInfo part2);
		NFAPartInfo OperateClosure(const NFAPartInfo part);
		NFAPartInfo GetInfo()const;
		const GraphList<int, string>* GetTransList()const;
		const set<int>* GetEndList()const;
		const map<int, NFAEndInfo>* GetEndInfoMap()const;
		void SetEndInfoMap(map<int, NFAEndInfo>& endInfoMap);
		void MakeTotalNFA(const int newStart, set<int>& newEndList);
		void Show()const;
	};
}

#endif