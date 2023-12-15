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
#include"GraphList.h"
#include"GraphList.cpp"
using namespace std;
using namespace toolkit;

namespace grammar {
	class DFA {
	private:
		string startWord;
		map<string, set<Production>> ref;
		map<Production, int> productionToNum;
		map<int, Production> numToProduction;
		//set<string> terminals;
		//set<string> nonterminals;
		
		map<string, set<string>> firstList;
		GraphList<DFAState, string> transList;
		vector<map<string, set<Trans>>> tableLR1;

		void PreProductions(const vector<Production>& grammar);
		void FormFirst(const Element& element);
		set<string> FormFirstRecursive(const Element& element);
		void PreWord(const set<string>& terminals, const set<string>& nonterminals);
		DFAState& GetClosure(DFAState& state);
	public:
		DFA(const vector<Production>& grammar, const set<string>& terminals, const set<string>& nonterminals, const string startWord) : transList(GraphList<DFAState, string>(true, 2048)), startWord(startWord) {
			this->PreProductions(grammar);
			this->PreWord(terminals, nonterminals);
		}
		
		set<string> GetFirst(const Element& word);
		set<string> GetFirst(vector<Element>& wordList);
		void ShowGrammar();
		void ShowFirstList();
		void ShowDFA();
		void AddTableLR1Edge(const int start, const int end, const Element& word);
		void AddTableLR1Vertex(const DFAState& state);
		void LR1();
		string FormatTrans(set<Trans>& set);
		void WriteTableLR1(ofstream& out, const set<string>& terminals, const set<string>& nonterminals);
		void ShowTableLR1(const set<string>& terminals, const set<string>& nonterminals);
		vector<string> ReadWords(const char* filename);
		bool GrammarAnalysis(vector<string>& input);
	};
}

#endif
