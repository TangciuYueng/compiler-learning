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
		string tokens;  //�ַ���
		vector<vector<set<int>>> detailedStateTransTable;  //��ϸ״̬ת����
		vector<vector<int>> stateTransTable;               //״̬ת����
		map<int, string> endMap;                           //��̬��ʶ������ʽ��ӳ��
		map<int, set<int>> detailedStateMap;               //״̬��ŵ�������ԭNFA״̬�Ӽ���ӳ��
		
		/*ԭNFA��Ϣ*/
		const GraphList<int, string>* originNFATransList;  //״̬ת��ͼ
		int originNFAStart;                                //��ʼ̬���
		const set<int>* originNFAEndList;                  //��̬�б�
		const map<int, NFAEndInfo>* originNFAEndInfoMap;   //��̬��Ϣ��
		
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