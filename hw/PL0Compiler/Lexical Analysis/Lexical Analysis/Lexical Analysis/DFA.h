#pragma once
#ifndef _DFA_H_
#define _DFA_H_
#include<set>
#include<queue>
#include<vector>
#include<map>
#include<fstream>
#include<climits>
#include<stack>
#include<iomanip>
#include"Auto.h"
#include"NormState.h"
#include"NFAEndInfo.h"
using namespace std;

namespace automat {
	class DFA : public Auto {
	private:
		string tokens;  //字符集
		vector<vector<set<int>>> detailedStateTransTable;  //详细状态转换表
		vector<vector<int>> stateTransTable;               //状态转换表
		map<int, string> endMap;                           //终态到识别正规式的映射
		map<int, set<int>> detailedStateMap;               //状态编号到包含的原NFA状态子集的映射
		
		/*原NFA信息*/
		const GraphList<int, string>* originNFATransList;  //状态转换图
		int originNFAStart;                                //起始态编号
		const set<int>* originNFAEndList;                  //终态列表
		const map<int, NFAEndInfo>* originNFAEndInfoMap;   //终态信息表
		
	public:
		DFA(
			const string tokens,
			const GraphList<int, string>* originNFATransList,
			const int originNFAStart,
			const set<int>* originNFAEndList,
			const map<int, NFAEndInfo>* originNFAEndInfoMap
		) : tokens(tokens), originNFATransList(originNFATransList), originNFAStart(originNFAStart), originNFAEndList(originNFAEndList), originNFAEndInfoMap(originNFAEndInfoMap) {}
		
		set<int> OriginNFATrans(const int init, const char ch)const;
		set<int> GetClosure(const set<int>& origin)const;
		void DetailedSubsetMethod();
		void ShowDetailedStateTransTable()const;
		void SubsetMethod();
		void ShowStateTransTable();
		void WriteStateTransTable(ofstream& out);
		bool LexicalAnalysis(string input);
	};
}

#endif