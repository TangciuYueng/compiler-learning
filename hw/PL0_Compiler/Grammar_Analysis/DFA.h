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
	/*识别活前缀、有展望符的DFA*/
	class DFA {
	private:
		string startWord;  //文法起始符
		map<string, set<int>> productionLeftRef;   //产生式左部到以该词为左部产生式编号集合的映射
		map<Production, int> productionToNum;      //产生式到编号的映射
		map<int, Production> numToProduction;      //编号到产生式的映射
		map<string, set<string>> firstList;        //各个词的FIRST()
		vector<map<string, set<Trans>>> tableLR1;  //LR(1)分析表

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
