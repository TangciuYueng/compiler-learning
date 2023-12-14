#include"DFA.h"
#include"Trans.h"
#include"Production.h"
#include"DFAProduction.h"

namespace grammar {
	void DFA::PreProductions(const vector<Production>& grammar) {
		for (Production production : grammar) {
			//1.按产生式左部加入ref映射
			auto place = this->ref.find(production.left);

			if (place != this->ref.end()) {
				(*place).second.insert(production);
			}
			else {
				set<Production> newSet = { production };
				this->ref.emplace(production.left, newSet);
			}

			//2.给产生式编号
			this->productionToNum.emplace(production, (int)this->productionToNum.size());
			this->numToProduction.emplace((int)this->numToProduction.size(), production);
		}
	}

	void DFA::FormFirst(const Element& element) {
		if (element.ifTerminal) {
			set<string> newSet = { element.word };
			this->firstList.emplace(element.word, newSet);
		}

		else {
			this->FormFirstRecursive(Element(element.word, false));
		}
	}

	set<string> DFA::FormFirstRecursive(const Element& element) {
		if (element.ifTerminal) {  //终结符则返回只有它自己的集合
			return set<string>({ element.word });
		}

		auto place = this->firstList.find(element.word);  //非终结符如果已经求出也直接返回
		if (place != this->firstList.end()) {
			return place->second;
		}

		set<string> newSet;
		for (Production production : this->ref.find(element.word)->second) {
			vector<Element>::iterator iter = production.right.begin();  //遍历产生式右边的元素

			while (1) {
				if (iter == production.right.end()) {  //可以到达末尾，说明一路未遇到终结符，或者非终结符均有空字，将空字加入FIRST()
					newSet.insert("EPSILON");
					break;
				}

				if (iter->ifTerminal) {  //遇到终结符，加入FIRST()，并检查下一个产生式
					newSet.insert(iter->word);
					break;
				}
				
				if (iter->word == element.word) {  //遇到自己，无可用信息，检查下一个产生式
					break;
				}

				set<string> checkSet = this->FormFirstRecursive(*iter);  //对于非自己的非终结符，递归求得它的FIRST()

				if (checkSet.find("EPSILON") != checkSet.end()) {  //若含有空字，则将除空字外其余加入当前的FIRST()，并检查下一个
					checkSet.erase("EPSILON");

					for (string terminal : checkSet) {
						newSet.insert(terminal);
					}
				}

				else {  //没有空字，加入当前FIRST()后直接检查下一产生式
					for (string terminal : checkSet) {
						newSet.insert(terminal);
					}

					break;
				}

				iter++;
			}
		}

		this->firstList.emplace(element.word, newSet);  //能运行到此说明该元素的FIRST()没求出，进行记录
		return newSet;
	}

	void DFA::PreWord(const set<string>& terminals, const set<string>& nonterminals) {
		for (string terminal : terminals) {
			this->FormFirst(Element(terminal, true));  //求FIRST()
		}

		for (string nonterminal : nonterminals) {
			this->FormFirst(Element(nonterminal, false));  //求FIRST()
		}
	}

	set<string> DFA::GetFirst(const Element& word) {
		auto pair = this->firstList.find(word.word);

		if (pair == this->firstList.end()) {
			return set<string>();
		}

		return pair->second;
	}

	set<string> DFA::GetFirst(vector<Element>& wordList) {
		if (wordList.size() == 0)
			return set<string>();

		if (wordList.size() == 1)
			return this->GetFirst(wordList[0]);

		set<string> ret;
		for (Element word : wordList) {
			if (word.ifTerminal) {
				ret.insert(word.word);
				return ret;
			}

			set<string> checkSet = this->GetFirst(word);  //对于非自己的非终结符，得到它的FIRST()

			if (checkSet.find("EPSILON") != checkSet.end()) {  //若含有空字，则将除空字外其余加入当前的FIRST()，并检查下一个
				checkSet.erase("EPSILON");

				for (string terminal : checkSet) {
					ret.insert(terminal);
				}
			}

			else {  //没有空字，加入当前FIRST()后直接返回
				for (string terminal : checkSet) {
					ret.insert(terminal);
				}

				return ret;
			}
		}

		//运行到此，说明一路未遇到终结符，或者非终结符均有空字，将空字加入FIRST()
		ret.insert("EPSION");
		return ret;
	}

	void DFA::ShowGrammar() {
		for (auto pair : this->ref) {
			for (Production production : pair.second) {
				cout << production << endl;
			}
		}
		cout << endl;
	}

	void DFA::ShowFirstList() {
		for (auto pair : this->firstList) {
			cout << "FIRST(" << pair.first << ") = ";

			for (string terminal : pair.second) {
				cout << terminal << ' ';
			}

			cout << endl;
		}
	}

	void DFA::ShowDFA() {
		for (int i = 0; i < this->transList.NumOfVertices(); i++)
		{
			DFAState vertex;
			this->transList.getVertex(vertex, i);
			cout << i << ":" << endl;
			cout << vertex << endl;
		}

		for (int i = 0; i < this->transList.NumOfVertices(); i++) {
			for (int j = 0; j < this->transList.NumOfVertices(); j++) {
				string edge;

				if (this->transList.getEdge(edge, i, j)) {
					cout << i << " --" << edge << "--> " << j << endl;
				}
			}
		}
	}

	void DFA::AddTableLR1Edge(const int start, const int end, const Element& word) {
		auto transPlace = this->tableLR1[start].find(word.word);
		if (transPlace != this->tableLR1[start].end()) {  //接收该符号时，目前有结果
			transPlace->second.insert(Trans(word.ifTerminal ? TransType::ACTION_SHIFT : TransType::GOTO, end));
		}
		else {
			set<Trans> newSet;
			newSet.insert(Trans(word.ifTerminal ? TransType::ACTION_SHIFT : TransType::GOTO, end));
			this->tableLR1[start].emplace(word.word, newSet);
		}
	}

	void DFA::AddTableLR1Vertex(const DFAState& state) {
		for (DFAProduction dfaProduction : state.productions) {
			if (dfaProduction.end()) {
				Production production = Production(dfaProduction.left, dfaProduction.right);
				const int pNum = this->productionToNum.find(production)->second;

				if (dfaProduction.left == this->startWord) {  //起始产生式，填入acc
					set<Trans> newSet;
					newSet.insert(Trans(TransType::ACTION_ACC, pNum));
					this->tableLR1[state.num].emplace("TERMINAL", newSet);
				}
				else {  //其它产生式，填入r
					for (string outlook : dfaProduction.outlook) {
						auto transPlace = this->tableLR1[state.num].find(outlook);
						if (transPlace != this->tableLR1[state.num].end()) {  //接收该符号时，目前有结果
							transPlace->second.insert(Trans(TransType::ACTION_REDUCE, pNum));
						}
						else {
							set<Trans> newSet;
							newSet.insert(Trans(TransType::ACTION_REDUCE, pNum));
							this->tableLR1[state.num].emplace(outlook, newSet);
						}
					}
				}
			}
		}
	}

	DFAState& DFA::GetClosure(DFAState& state) {
		for (size_t i = 0; i < state.productions.size(); i++) {
			if (state.productions[i].end() || state.productions[i].GetPoint().ifTerminal)
				continue;

			set<string> newOutlook;
			int next = state.productions[i].point + 1;
			if (state.productions[i].nextEnd()) {
				for (string outlook : state.productions[i].outlook) {
					newOutlook.insert(outlook);
				}
			}
			else {
				vector<Element> part;

				for (size_t j = state.productions[i].point + 1; j < state.productions[i].right.size(); j++)
				{
					part.push_back(state.productions[i].right[j]);
				}

				for (string outlook : state.productions[i].outlook) {  //这个部分也要加上，即带上原产生式的展望符一起求
					part.push_back(Element(outlook, true));  //还需改进：应该是原产生式的各展望符被单带一次进行求解
				}

				newOutlook = this->GetFirst(part);
				newOutlook.erase("EPSILON");
			}

			//遍历所有*point打头的产生式，化为DFA产生式并加上求得的outlook，加入当前状态
			for (Production production : this->ref.find(state.productions[i].GetPoint().word)->second) {
				DFAProduction newDp = DFAProduction(production.left, production.right, newOutlook);
				bool ifThisAdd = true;

				for (DFAProduction dp : state.productions) {
					if (dp == newDp) {
						ifThisAdd = false;
						break;
					}
				}

				if (ifThisAdd) {
					state.productions.push_back(newDp);
				}
			}
		}

		return state;
	}

	void DFA::LR1() {
		//1.以第一个产生式构建初始状态（即outlook只有#）
		//	1.构建闭包
		//		遍历现有DFA产生式
		//		*point是终结符或已到末尾则略过
		//      *point如果是非终结符，查看*(point+1)，求FIRST()（到末尾则是当前产生式的outlook）
		//      将所有*point打头的产生式，加上求得的FIRST()集合，添加到新状态中
		//	2.构建状态迁移
		//      准备一个从当前状态转移出的状态列表（小openlist，接受符号到接受该符号后DFAState映射）
		//      遍历状态所有产生式
		//      *point到末尾则略过，否则point向后移一格后加入新state
		//		遍历完后新state加入小openlist
		//      遍历小openlist，求闭包后若已经存在，则只加边；不存在则加点，并加入openlist
		//      当前状态加入closelist
		DFAState initState;
		queue<DFAState> openlist;
		set<DFAState> closelist;

		for (Production production : this->ref.find(this->startWord)->second) {
			initState.num = 0;
			set<string> initOutLook = set<string>({ "TERMINAL" });
			initState.productions.push_back(DFAProduction(production.left, production.right, initOutLook));
		}

		this->GetClosure(initState);
		this->transList.insertVertex(initState);
		this->tableLR1.push_back(map<string, set<Trans>>());
		openlist.push(initState);
		
		while (!openlist.empty()) {
			DFAState current = openlist.front();
			openlist.pop();
			map<Element, DFAState> prepOpenlist;

			for (DFAProduction dfaProduction : current.productions) {
				if (dfaProduction.end()) {
					continue;
				}
				else {
					Element recv = dfaProduction.GetPoint();
					auto place = prepOpenlist.find(dfaProduction.GetPoint());  //取移入前，要接受的符号
					dfaProduction.add();  //点后移
					

					if (place != prepOpenlist.end()) {  //小openlist有接收该符号的状态
						place->second.productions.push_back(dfaProduction);
					}
					else {  //小openlist没有接收该符号的状态
						vector<DFAProduction> newProductions = { dfaProduction };
						DFAState newState(-1, newProductions);
						prepOpenlist.emplace(recv, newState);
					}
				}
			}

			for (auto pair : prepOpenlist) {
				Element edge = pair.first;
				DFAState state = pair.second;
				this->GetClosure(state);
				int startPos = this->transList.getVertexPos(current);
				int endPos = this->transList.getVertexPos(state);
				
				if (endPos != -1) {  //状态已经存在
					this->transList.insertEdge(startPos, endPos, edge.word);
					this->AddTableLR1Edge(startPos, endPos, edge);  //分析表添加迁移项
					if (state.num == 0)
						this->AddTableLR1Vertex(state);  //初始状态已经存在，需要分析表添加归约项
				}
				else {  //状态不存在
					state.num = this->transList.NumOfVertices();  //检查当前结点时，该结点未在图中，编号应当是顶点数
					this->transList.insertVertex(state);
					endPos = this->transList.getVertexPos(state);
					this->transList.insertEdge(startPos, endPos, edge.word);
					openlist.push(state);
					this->tableLR1.push_back(map<string, set<Trans>>());
					this->AddTableLR1Edge(startPos, endPos, edge);  //分析表添加迁移项
					this->AddTableLR1Vertex(state);                 //分析表添加归约项
				}
			}

			closelist.insert(current);
		}
	}

	string DFA::FormatTrans(set<Trans>& set) {
		string ret;
		auto iter = set.begin();

		while (iter != set.end()) {
			if (iter == set.begin()) {
				ret += iter->Format();
			}
			else {
				ret += '/' + iter->Format();
			}

			iter++;
		}

		return ret;
	}

	void DFA::ShowTableLR1(const set<string>& terminals, const set<string>& nonterminals) {
		set<int> dupStates;
		const int width = 10;

		cout << "   ";

		for (string terminal : terminals) {
			cout << setw(width) << setfill(' ') << setiosflags(ios::left) << terminal;
		}

		for (string nonterminal : nonterminals) {
			cout << setw(width) << setfill(' ') << setiosflags(ios::left) << nonterminal;
		}

		cout << endl;

		for (int i = 0; i < this->tableLR1.size(); i++) {
			DFAState state;
			this->transList.getVertex(state, i);
			cout << setw(4) << setfill(' ') << setiosflags(ios::left) << state.num;

			for (string terminal : terminals) {
				auto place = this->tableLR1[i].find(terminal);

				if (place != this->tableLR1[i].end()) {
					if (place->second.size() != 1)
						dupStates.insert(i);

					cout << setw(width) << setfill(' ') << setiosflags(ios::left) << this->FormatTrans(place->second);
				}
				else {
					for (int j = 0; j < width; j++) {
						cout << ' ';
					}
				}
			}

			for (string nonterminal : nonterminals) {
				auto place = this->tableLR1[i].find(nonterminal);

				if (place != this->tableLR1[i].end()) {
					if (place->second.size() != 1)
						dupStates.insert(i);

					cout << setw(width) << setfill(' ') << setiosflags(ios::left) << this->FormatTrans(place->second);
				}
				else {
					for (int j = 0; j < width; j++) {
						cout << ' ';
					}
				}
			}

			cout << endl;
		}

		cout << "-------------------------------" << endl;
		for (int state : dupStates) {
			cout << state << ' ';
		}
		cout << endl << "-------------------------------" << endl;
	}

	void DFA::WriteTableLR1(ofstream& out, const set<string>& terminals, const set<string>& nonterminals) {
		set<int> dupStates;
		const int width = 10;

		out << "id,";

		for (string terminal : terminals) {
			out << terminal << ',';
		}

		for (string nonterminal : nonterminals) {
			out << nonterminal << ',';
		}

		out << endl;

		for (int i = 0; i < this->tableLR1.size(); i++) {
			out << i << ',';

			for (string terminal : terminals) {
				auto place = this->tableLR1[i].find(terminal);

				if (place != this->tableLR1[i].end()) {
					if (place->second.size() != 1)
						dupStates.insert(i);

					//cout << i << "状态面临" << terminal << "操作数：" << place->second.size() << endl;
					out << this->FormatTrans(place->second) << ',';
				}
				else {
					out << ',';
				}
			}

			for (string nonterminal : nonterminals) {
				auto place = this->tableLR1[i].find(nonterminal);

				if (place != this->tableLR1[i].end()) {
					if (place->second.size() != 1)
						dupStates.insert(i);

					//cout << i << "状态面临" << nonterminal << "操作数：" << place->second.size() << endl;
					out << this->FormatTrans(place->second) << ',';
				}
				else {
					out << ',';
				}
			}

			out << endl;
		}

		out << "冲突状态,";
		for (int state : dupStates) {
			out << state << ',';
		}
	}

	vector<string> DFA::ReadWords(const char* filename) {
		ifstream in(filename);
		int count = 0;

		if (!in) {
			cout << "读取" << filename << "失败！" << endl;
			return vector<string>();
		}

		vector<string> ret;
		string word;
		in.seekg(count);
		while (in.peek() != EOF) {
			const char ch = in.peek();

			if (ch == '\n') {
				in.seekg(++count);

				if (word.length() != 0) {
					ret.push_back(word);
					word.clear();
				}
			}
			else {
				word += ch;
				in.seekg(++count);
			}
		}

		return ret;
	}

	bool DFA::GrammarAnalysis(vector<string>& input) {
		stack<int> stateRecord;
		stack<string> wordRecord;
		stateRecord.push(0);
		wordRecord.push("TERMINAL");

		set<int> ambiguousShift = {};
		set<int> ambiguousReduce = {};

		vector<string>::iterator iter = input.begin();
		while (iter != input.end()) {
			vector<string> prep;
			while (!wordRecord.empty()) {
				prep.push_back(wordRecord.top());
				wordRecord.pop();
			}
			vector<string>::iterator it = prep.end();

			do {
				it--;
				cout << *it << ' ';
				wordRecord.push(*it);
			} while (it > prep.begin());
			cout << endl;

			const int top = stateRecord.top();
			cout << top << "面临词语：" << *iter << endl;
			const Trans* op = NULL;
			auto operations = this->tableLR1[top].find(*iter);

			if (operations == this->tableLR1[top].end()) {
				cout << "throw GrammarAnalysisError" << endl;
				return false;
			}

			if (ambiguousShift.find(top) != ambiguousShift.end()) {
				for (Trans trans : operations->second) {
					if (trans.type == TransType::ACTION_SHIFT) {
						op = &trans;
					}
				}
			}
			else if (ambiguousReduce.find(top) != ambiguousReduce.end()) {
				for (Trans trans : operations->second) {
					if (trans.type == TransType::ACTION_REDUCE) {
						op = &trans;
					}
				}
			}
			else {
				op = &(*(this->tableLR1[top].find(*iter)->second.begin()));
			}

			switch (op->type) {
			case TransType::ACTION_SHIFT:
				stateRecord.push(op->dest);
				wordRecord.push(*iter);
				cout << "移进" << *iter << "迁移到" << op->dest << endl;
				break;
			case TransType::GOTO:
				stateRecord.push(op->dest);
				wordRecord.push(*iter);
				cout << "移进" << *iter << "迁移到" << op->dest << endl;
				break;
			case TransType::ACTION_REDUCE:
				{
					Production production = this->numToProduction.find(op->dest)->second;
					const string word = production.left;
					size_t reduceLen = production.right.size();

					if (reduceLen == 1 && production.right[0].word == "EPSILON")  //产生式右边只有空字时，尽管归约时栈不必弹出，但right的长度为1（存了EPSILON本身），需要设为0
						reduceLen = 0;

					for (size_t i = 0; i < reduceLen; i++) {
						stateRecord.pop();
						wordRecord.pop();
					}

					const int newTop = stateRecord.top();
					stateRecord.push(this->tableLR1[newTop].find(word)->second.begin()->dest);
					wordRecord.push(word);
					cout << "用" << op->dest << "号产生式归约到" << stateRecord.top() << endl;
					iter--;
				}
				break;
			case TransType::ACTION_ACC:
				cout << "Grammar Analysis Success" << endl;
				break;
			}

			iter++;
		}

		return true;
	}
}