#ifndef _DFA_H_
#define _DFA_H_
#include<vector>
#include<map>
#include<set>
#include<queue>
#include<iomanip>
#include<fstream>
#include<stack>
#include"DFAState.h"
#include"Trans.h"
#include"Production.h"
#include"DFAProduction.h"
using namespace std;

namespace grammar {
	/*ʶ���ǰ׺����չ������DFA*/
	class DFA {
	private:
		string startWord;  //�ķ���ʼ��
		map<string, set<int>> productionLeftRef;   //����ʽ�󲿵��Ըô�Ϊ�󲿲���ʽ��ż��ϵ�ӳ��
		map<Production, int> productionToNum;      //����ʽ����ŵ�ӳ��
		map<int, Production> numToProduction;      //��ŵ�����ʽ��ӳ��
		map<string, set<string>> firstList;        //�����ʵ�FIRST()
		vector<map<string, set<Trans>>> tableLR1;  //LR(1)������

		void PreProductions(const vector<Production>& grammar);
		void PreWord(const set<string>& terminals, const set<string>& nonterminals);
		void FormFirst(const Element& element);
		set<string> FormFirstRecursive(const Element& element);
		set<string> GetFirst(const Element& word);
		set<string> GetFirst(const vector<Element>& wordList);
		void GetClosure(DFAState& state);
		void AddTableLR1Edge(const int start, const int end, const Element& word);
		void AddTableLR1Vertex(const DFAState& state);
		string FormatTrans(const set<Trans>& set);
	public:
		DFA(const vector<Production>& grammar, const set<string>& terminals, const set<string>& nonterminals, const string startWord) : startWord(startWord) {
			this->PreProductions(grammar);
			this->PreWord(terminals, nonterminals);
		}
		
		void ShowFirstList();
		void LR1();
		void ShowTableLR1(const set<string>& terminals, const set<string>& nonterminals);
		void WriteTableLR1(ofstream& out, const set<string>& terminals, const set<string>& nonterminals);
		vector<string> ReadWords(const char* filename);
		bool GrammarAnalysis(vector<string>& input);
	};
}

#endif
