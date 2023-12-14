#include"DFA.h"
#include"Trans.h"
#include"Production.h"
#include"DFAProduction.h"

namespace grammar {
	void DFA::PreProductions(const vector<Production>& grammar) {
		for (Production production : grammar) {
			//1.������ʽ�󲿼���refӳ��
			auto place = this->ref.find(production.left);

			if (place != this->ref.end()) {
				(*place).second.insert(production);
			}
			else {
				set<Production> newSet = { production };
				this->ref.emplace(production.left, newSet);
			}

			//2.������ʽ���
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
		if (element.ifTerminal) {  //�ս���򷵻�ֻ�����Լ��ļ���
			return set<string>({ element.word });
		}

		auto place = this->firstList.find(element.word);  //���ս������Ѿ����Ҳֱ�ӷ���
		if (place != this->firstList.end()) {
			return place->second;
		}

		set<string> newSet;
		for (Production production : this->ref.find(element.word)->second) {
			vector<Element>::iterator iter = production.right.begin();  //��������ʽ�ұߵ�Ԫ��

			while (1) {
				if (iter == production.right.end()) {  //���Ե���ĩβ��˵��һ·δ�����ս�������߷��ս�����п��֣������ּ���FIRST()
					newSet.insert("EPSILON");
					break;
				}

				if (iter->ifTerminal) {  //�����ս��������FIRST()���������һ������ʽ
					newSet.insert(iter->word);
					break;
				}
				
				if (iter->word == element.word) {  //�����Լ����޿�����Ϣ�������һ������ʽ
					break;
				}

				set<string> checkSet = this->FormFirstRecursive(*iter);  //���ڷ��Լ��ķ��ս�����ݹ��������FIRST()

				if (checkSet.find("EPSILON") != checkSet.end()) {  //�����п��֣��򽫳�������������뵱ǰ��FIRST()���������һ��
					checkSet.erase("EPSILON");

					for (string terminal : checkSet) {
						newSet.insert(terminal);
					}
				}

				else {  //û�п��֣����뵱ǰFIRST()��ֱ�Ӽ����һ����ʽ
					for (string terminal : checkSet) {
						newSet.insert(terminal);
					}

					break;
				}

				iter++;
			}
		}

		this->firstList.emplace(element.word, newSet);  //�����е���˵����Ԫ�ص�FIRST()û��������м�¼
		return newSet;
	}

	void DFA::PreWord(const set<string>& terminals, const set<string>& nonterminals) {
		for (string terminal : terminals) {
			this->FormFirst(Element(terminal, true));  //��FIRST()
		}

		for (string nonterminal : nonterminals) {
			this->FormFirst(Element(nonterminal, false));  //��FIRST()
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

			set<string> checkSet = this->GetFirst(word);  //���ڷ��Լ��ķ��ս�����õ�����FIRST()

			if (checkSet.find("EPSILON") != checkSet.end()) {  //�����п��֣��򽫳�������������뵱ǰ��FIRST()���������һ��
				checkSet.erase("EPSILON");

				for (string terminal : checkSet) {
					ret.insert(terminal);
				}
			}

			else {  //û�п��֣����뵱ǰFIRST()��ֱ�ӷ���
				for (string terminal : checkSet) {
					ret.insert(terminal);
				}

				return ret;
			}
		}

		//���е��ˣ�˵��һ·δ�����ս�������߷��ս�����п��֣������ּ���FIRST()
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
		if (transPlace != this->tableLR1[start].end()) {  //���ո÷���ʱ��Ŀǰ�н��
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

				if (dfaProduction.left == this->startWord) {  //��ʼ����ʽ������acc
					set<Trans> newSet;
					newSet.insert(Trans(TransType::ACTION_ACC, pNum));
					this->tableLR1[state.num].emplace("TERMINAL", newSet);
				}
				else {  //��������ʽ������r
					for (string outlook : dfaProduction.outlook) {
						auto transPlace = this->tableLR1[state.num].find(outlook);
						if (transPlace != this->tableLR1[state.num].end()) {  //���ո÷���ʱ��Ŀǰ�н��
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

				for (string outlook : state.productions[i].outlook) {  //�������ҲҪ���ϣ�������ԭ����ʽ��չ����һ����
					part.push_back(Element(outlook, true));  //����Ľ���Ӧ����ԭ����ʽ�ĸ�չ����������һ�ν������
				}

				newOutlook = this->GetFirst(part);
				newOutlook.erase("EPSILON");
			}

			//��������*point��ͷ�Ĳ���ʽ����ΪDFA����ʽ��������õ�outlook�����뵱ǰ״̬
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
		//1.�Ե�һ������ʽ������ʼ״̬����outlookֻ��#��
		//	1.�����հ�
		//		��������DFA����ʽ
		//		*point���ս�����ѵ�ĩβ���Թ�
		//      *point����Ƿ��ս�����鿴*(point+1)����FIRST()����ĩβ���ǵ�ǰ����ʽ��outlook��
		//      ������*point��ͷ�Ĳ���ʽ��������õ�FIRST()���ϣ���ӵ���״̬��
		//	2.����״̬Ǩ��
		//      ׼��һ���ӵ�ǰ״̬ת�Ƴ���״̬�б�Сopenlist�����ܷ��ŵ����ܸ÷��ź�DFAStateӳ�䣩
		//      ����״̬���в���ʽ
		//      *point��ĩβ���Թ�������point�����һ��������state
		//		���������state����Сopenlist
		//      ����Сopenlist����հ������Ѿ����ڣ���ֻ�ӱߣ���������ӵ㣬������openlist
		//      ��ǰ״̬����closelist
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
					auto place = prepOpenlist.find(dfaProduction.GetPoint());  //ȡ����ǰ��Ҫ���ܵķ���
					dfaProduction.add();  //�����
					

					if (place != prepOpenlist.end()) {  //Сopenlist�н��ո÷��ŵ�״̬
						place->second.productions.push_back(dfaProduction);
					}
					else {  //Сopenlistû�н��ո÷��ŵ�״̬
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
				
				if (endPos != -1) {  //״̬�Ѿ�����
					this->transList.insertEdge(startPos, endPos, edge.word);
					this->AddTableLR1Edge(startPos, endPos, edge);  //���������Ǩ����
					if (state.num == 0)
						this->AddTableLR1Vertex(state);  //��ʼ״̬�Ѿ����ڣ���Ҫ��������ӹ�Լ��
				}
				else {  //״̬������
					state.num = this->transList.NumOfVertices();  //��鵱ǰ���ʱ���ý��δ��ͼ�У����Ӧ���Ƕ�����
					this->transList.insertVertex(state);
					endPos = this->transList.getVertexPos(state);
					this->transList.insertEdge(startPos, endPos, edge.word);
					openlist.push(state);
					this->tableLR1.push_back(map<string, set<Trans>>());
					this->AddTableLR1Edge(startPos, endPos, edge);  //���������Ǩ����
					this->AddTableLR1Vertex(state);                 //��������ӹ�Լ��
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

					//cout << i << "״̬����" << terminal << "��������" << place->second.size() << endl;
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

					//cout << i << "״̬����" << nonterminal << "��������" << place->second.size() << endl;
					out << this->FormatTrans(place->second) << ',';
				}
				else {
					out << ',';
				}
			}

			out << endl;
		}

		out << "��ͻ״̬,";
		for (int state : dupStates) {
			out << state << ',';
		}
	}

	vector<string> DFA::ReadWords(const char* filename) {
		ifstream in(filename);
		int count = 0;

		if (!in) {
			cout << "��ȡ" << filename << "ʧ�ܣ�" << endl;
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
			cout << top << "���ٴ��" << *iter << endl;
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
				cout << "�ƽ�" << *iter << "Ǩ�Ƶ�" << op->dest << endl;
				break;
			case TransType::GOTO:
				stateRecord.push(op->dest);
				wordRecord.push(*iter);
				cout << "�ƽ�" << *iter << "Ǩ�Ƶ�" << op->dest << endl;
				break;
			case TransType::ACTION_REDUCE:
				{
					Production production = this->numToProduction.find(op->dest)->second;
					const string word = production.left;
					size_t reduceLen = production.right.size();

					if (reduceLen == 1 && production.right[0].word == "EPSILON")  //����ʽ�ұ�ֻ�п���ʱ�����ܹ�Լʱջ���ص�������right�ĳ���Ϊ1������EPSILON��������Ҫ��Ϊ0
						reduceLen = 0;

					for (size_t i = 0; i < reduceLen; i++) {
						stateRecord.pop();
						wordRecord.pop();
					}

					const int newTop = stateRecord.top();
					stateRecord.push(this->tableLR1[newTop].find(word)->second.begin()->dest);
					wordRecord.push(word);
					cout << "��" << op->dest << "�Ų���ʽ��Լ��" << stateRecord.top() << endl;
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