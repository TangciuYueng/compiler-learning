#include"DFA.h"

namespace grammar {
	/***************************************************************************
	  �������ƣ�PreProductions
	  ��    �ܣ����ķ��еĲ���ʽ����Ԥ����
	  ���������const vector<Production>& grammar���ķ���ֻ����
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void DFA::PreProductions(const vector<Production>& grammar) {
		int num = 0;

		for (Production production : grammar) {
			//1.������ʽ���
			this->productionToNum.emplace(production, num);
			this->numToProduction.emplace(num, production);

			//2.������ʽ�󲿼���refӳ��
			auto place = this->productionLeftRef.find(production.left);

			if (place != this->productionLeftRef.end()) {
				(*place).second.insert(num);
			}
			else {
				set<int> newSet = { num };
				this->productionLeftRef.emplace(production.left, newSet);
			}

			num++;
		}
	}

	/***************************************************************************
	  �������ƣ�PreWord
	  ��    �ܣ����ķ��еĴʽ���Ԥ����
	  ���������const set<string>& terminals���ս�����ϣ�ֻ����
	            const set<string>& nonterminals�����ս�����ϣ�ֻ����
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void DFA::PreWord(const set<string>& terminals, const set<string>& nonterminals) {
		for (string terminal : terminals) {
			this->FormFirst(Element(terminal, true));  //��FIRST()
		}

		for (string nonterminal : nonterminals) {
			this->FormFirst(Element(nonterminal, false));  //��FIRST()
		}
	}

	/***************************************************************************
	  �������ƣ�FormFirst
	  ��    �ܣ����һ���ʵ�FIRST()
	  ���������const Element& element��Ҫ��FIRST()�Ĵʣ�ֻ����
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void DFA::FormFirst(const Element& element) {
		if (element.ifTerminal) {  //������ս������һ��ֻ�����Լ��ļ��ϼ���
			set<string> newSet = { element.word };
			this->firstList.emplace(element.word, newSet);
		}

		else {  //����ݹ������ս����FIRST()
			this->FormFirstRecursive(element);
		}
	}

	/***************************************************************************
	  �������ƣ�FormFirstRecursive
	  ��    �ܣ����һ���ʵ�FIRST()���ݹ鲿�֣�
	  ���������const Element& element��Ҫ��FIRST()�Ĵʣ�ֻ����
	  �� �� ֵ��set<string>���ôʵ�FIRST()
	  ˵    ����1.���㷨�޷�������ս��������ݹ���ķ�
	            2.���˵�����������дPL0�Ĳ���ʽʱ��ȫ��������ݹ�
				3.��Ҫ������׳�ԣ�Ҳ�ɲ������϶��·�����������ݹ�ķ���
	***************************************************************************/
	set<string> DFA::FormFirstRecursive(const Element& element) {
		if (element.ifTerminal) {  //�ս���򷵻�ֻ�����Լ��ļ���
			return set<string>({ element.word });
		}

		auto place = this->firstList.find(element.word);  //���ս������Ѿ����Ҳֱ�ӷ���
		if (place != this->firstList.end()) {
			return place->second;
		}

		set<string> newSet;  //�����FIRST()

		//��FIRST()��Ҫ�鿴�����Ըô�Ϊ�󲿵Ĳ���ʽ
		for (int proNum : this->productionLeftRef.find(element.word)->second) {
			Production production = this->numToProduction.find(proNum)->second;  //������Ų鵽����ʽ
			vector<Element>::iterator iter = production.right.begin();           //��������ʽ�ұߵ�Ԫ��

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

		this->firstList.emplace(element.word, newSet);  //�����е���˵����Ԫ�ص�FIRST()����������м�¼
		return newSet;
	}

	/***************************************************************************
	  �������ƣ�GetFirst
	  ��    �ܣ���ѯһ���ʵ�FIRST()
	  ���������const Element& element��Ҫ��FIRST()�Ĵʣ�ֻ����
	  �� �� ֵ��set<string>���ôʵ�FIRST()
	  ˵    ������������������������д�FIRST()�Ļ�����
	***************************************************************************/
	set<string> DFA::GetFirst(const Element& word) {
		auto pair = this->firstList.find(word.word);

		if (pair == this->firstList.end()) {
			return set<string>();
		}

		return pair->second;
	}

	/***************************************************************************
	  �������ƣ�GetFirst
	  ��    �ܣ���ѯһ�����ӵ�FIRST()
	  ���������const vector<Element>& wordList��Ҫ��FIRST()�ľ��ӣ�ֻ����
	  �� �� ֵ��set<string>���ôʵ�FIRST()
	  ˵    ������������������������д�FIRST()�Ļ�����
	***************************************************************************/
	set<string> DFA::GetFirst(const vector<Element>& wordList) {
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

	/***************************************************************************
	  �������ƣ�ShowFirstList
	  ��    �ܣ���ӡ���дʵ�FIRST()
	  �����������
	  �� �� ֵ����
	  ˵    ������������������������д�FIRST()�Ļ�����
	***************************************************************************/
	void DFA::ShowFirstList() {
		for (auto pair : this->firstList) {
			cout << "FIRST(" << pair.first << ") = ";

			for (string terminal : pair.second) {
				cout << terminal << ' ';
			}

			cout << endl;
		}
	}

	/***************************************************************************
	  �������ƣ�GetClosure
	  ��    �ܣ���һ������LR(1)��Ŀ���ıհ�
	  ���������DFAState& state��LR(1)��Ŀ��
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void DFA::GetClosure(DFAState& state) {
		for (size_t i = 0; i < state.productions.size(); i++) {  //����ÿ����Ŀ����ʹ���¼����Ҳ�ܱ�������
			//������Ŀ�ǹ�Լ��Ŀ�������Ŀֻ�ܽ����ս����������
			if (state.productions[i].end() || state.productions[i].GetPoint().ifTerminal)
				continue;

			set<string> newOutlook;                     //����Ŀ��չ������
			int next = state.productions[i].point + 1;  //ȡ�ò���ʽԲ�����һ��λ��

			//�������һ��λ��Բ�㵽ͷ�ˣ�������Ŀ��չ���������ǵ�ǰ��Ŀ��
			if (state.productions[i].nextEnd()) {
				for (string outlook : state.productions[i].outlook) {
					newOutlook.insert(outlook);
				}
			}
			//�����û��ͷ
			else {
				for (string outlook : state.productions[i].outlook) {
					vector<Element> part;

					//ȡԲ�����һλ�󣬲���ʽ�Ҳ�ʣ�µĲ���
					for (size_t j = state.productions[i].point + 1; j < state.productions[i].right.size(); j++)
					{
						part.push_back(state.productions[i].right[j]);
					}

					//ͬʱ����ԭ����ʽ��չ����
					part.push_back(Element(outlook, true));

					//����ò��ֵ�FIRST()��������Ŀ��չ��������
					set<string> partFirst = this->GetFirst(part);
					set_union(partFirst.begin(), partFirst.end(), newOutlook.begin(), newOutlook.end(),
						inserter(newOutlook, newOutlook.begin()));
				}
				
				newOutlook.erase("EPSILON");  //չ�����޿���
			}

			//���չ����������ò���ʽ�ܽ��ܷ��ս��N
			//��������N��ͷ�Ĳ���ʽ����ΪLR(1)��Ŀ��������õ�outlook�����뵱ǰ״̬
			const string left = state.productions[i].GetPoint().word;

			for (int proNum : this->productionLeftRef.find(left)->second) {
				Production production = this->numToProduction.find(proNum)->second;
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
	}

	/***************************************************************************
	  �������ƣ�LR1
	  ��    �ܣ�ʹ��LR(1)��������ȡ�ķ���LR(1)������
	  �����������
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void DFA::LR1() {
		DFAState initState;            //��ʼ״̬
		queue<DFAState> openlist;      //��չ�б�
		map<vector<DFAProduction>, int> existList;  //����״̬��

		//���ķ���ʼ��Ϊ�󲿵Ĳ���ʽ����ֻ�н�������չ��������Ϊһ��LR(1)��Ŀ�������ʼ״̬����������հ�
		Production production = this->numToProduction.find(0)->second;
		initState.num = 0;
		set<string> initOutLook = set<string>({ "TERMINAL" });
		initState.productions.push_back(DFAProduction(production.left, production.right, initOutLook));
		this->GetClosure(initState);

		existList.emplace(initState.productions, 0);          //����ʼ״̬�����뵽����״̬�б���
		this->tableLR1.push_back(map<string, set<Trans>>());  //LR(1)����������µ�һ��
		openlist.push(initState);  //����ʼ״̬������չ�б�

		while (!openlist.empty()) {
			DFAState current = openlist.front();
			openlist.pop();
			map<Element, DFAState> prepOpenlist;  //��ǰ״̬��ת�Ƶ�������״̬���ϣ����Сopenlist

			//������ǰ״̬���в���ʽ
			for (DFAProduction dfaProduction : current.productions) {
				if (dfaProduction.end()) {  //��Բ���ѵ�ͷ�������
					continue;
				}

				Element recv = dfaProduction.GetPoint();  //ȡ��LR(1)��ĿҪ���յķ���
				auto place = prepOpenlist.find(dfaProduction.GetPoint());
				dfaProduction.add();  //�����

				//����ǰ״̬���ո÷��ŵ�״̬�Ѿ���Сopenlist�У���ֱ�ӽ�Բ���ƶ����LR(1)��Ŀ�����״̬
				if (place != prepOpenlist.end()) {
					place->second.productions.push_back(dfaProduction);
				}
				//����ǰ״̬���ո÷��ŵ�״̬����Сopenlist�У���Сopenlist����һ��״̬������ֻ��Բ���ƶ���ĵ�ǰLR(1)��Ŀ
				else {
					vector<DFAProduction> newProductions = { dfaProduction };
					DFAState newState(-1, newProductions);
					prepOpenlist.emplace(recv, newState);
				}
			}

			//����Сopenlist
			for (auto pair : prepOpenlist) {
				Element edge = pair.first;     //���յķ���
				DFAState state = pair.second;  //״̬
				this->GetClosure(state);       //��״̬��հ�
				int startPos = existList.find(current.productions)->second;  //��ǰ״̬һ���Ѿ�����
				auto end = existList.find(state.productions);

				//״̬�ڵ�ǰDFA���Ѿ�����
				if (end != existList.end()) {
					this->AddTableLR1Edge(startPos, end->second, edge);  //���������Ǩ�ƶ���
					if (state.num == 0)
						this->AddTableLR1Vertex(state);  //��ʼ״̬�Ѿ����ڣ���Ҫ��������ӹ�Լ����
				}
				//״̬�ڵ�ǰDFA�в�����
				else {
					state.num = existList.size();         //��״̬���
					cout << "��״̬��" << state.num << endl;
					existList.emplace(state.productions, state.num);  //����״̬���м�¼
					openlist.push(state);                 //����״̬������չ�б�
					this->tableLR1.push_back(map<string, set<Trans>>());  //LR(1)����������һ��
					this->AddTableLR1Edge(startPos, state.num, edge);     //���������Ǩ�ƶ���
					this->AddTableLR1Vertex(state);                       //��������ӹ�Լ����
				}
			}
		}
	}

	/***************************************************************************
	  �������ƣ�AddTableLR1Edge
	  ��    �ܣ���LR(1)���������Ǩ�ƶ���
	  ���������const int start����ʼ״̬��ֻ����
				const int end��Ŀ��״̬��ֻ����
				const Element& word�����մʣ�ֻ����
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void DFA::AddTableLR1Edge(const int start, const int end, const Element& word) {
		auto transPlace = this->tableLR1[start].find(word.word);  //��LR(1)���������ʼ״̬��һ���ҵ�����word��һ�ж�Ӧ�Ķ�������

		//���ö������ϴ��ڣ������ö���
		if (transPlace != this->tableLR1[start].end()) {
			transPlace->second.insert(Trans(word.ifTerminal ? TransType::ACTION_SHIFT : TransType::GOTO, end));  //���ݽ��մ��Ƿ�Ϊ�ս��������������shift����goto
		}
		//���ö������ϲ����ڣ��򴴽��ü��ϣ�ֻ�е�ǰһ������
		else {
			set<Trans> newSet;
			newSet.insert(Trans(word.ifTerminal ? TransType::ACTION_SHIFT : TransType::GOTO, end));  //���ݽ��մ��Ƿ�Ϊ�ս��������������shift����goto
			this->tableLR1[start].emplace(word.word, newSet);
		}
	}

	/***************************************************************************
	  �������ƣ�AddTableLR1Vertex
	  ��    �ܣ���LR(1)��������ӹ�Լ����
	  ���������const DFAState& state�������״̬��ֻ����
	  �� �� ֵ����
	  ˵    ������
	***************************************************************************/
	void DFA::AddTableLR1Vertex(const DFAState& state) {
		for (DFAProduction dfaProduction : state.productions) {  //������״̬����ʽ
			if (dfaProduction.end()) {  //ֻ��Բ�㵽ͷ��LR(1)��Ŀ���ǹ�Լ��Ŀ
				Production production = Production(dfaProduction.left, dfaProduction.right);
				const int pNum = this->productionToNum.find(production)->second;

				//��ʼ����ʽ��acc
				if (dfaProduction.left == this->startWord) {
					set<Trans> newSet;
					newSet.insert(Trans(TransType::ACTION_ACC, pNum));
					this->tableLR1[state.num].emplace("TERMINAL", newSet);
				}
				//��������ʽ��reduce
				else {
					for (string outlook : dfaProduction.outlook) {  //��ÿ��չ��������ӹ�Լ����
						auto transPlace = this->tableLR1[state.num].find(outlook);

						//�������ϴ��ڣ�ֱ�����
						if (transPlace != this->tableLR1[state.num].end()) {
							transPlace->second.insert(Trans(TransType::ACTION_REDUCE, pNum));
						}
						//�������ϲ����ڣ�������������
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

	/***************************************************************************
	  �������ƣ�FormatTrans
	  ��    �ܣ����������ϸ�ʽ�����ַ������������
	  ���������const set<Trans>& set������ʽ���Ķ������ϣ�ֻ����
	  �� �� ֵ��string����ʽ������ַ���
	  ˵    ������
	***************************************************************************/
	string DFA::FormatTrans(const set<Trans>& set) {
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

	/***************************************************************************
	  �������ƣ�ShowTableLR1
	  ��    �ܣ���ʽ����ӡLR(1)������
	  ���������const set<string>& terminals���ս�����ϣ�ֻ����
				const set<string>& nonterminals�����ս�����ϣ�ֻ����
	  �� �� ֵ����
	  ˵    ����������ͬʱͳ���˳�ͻ״̬����Ϊ������
	***************************************************************************/
	void DFA::ShowTableLR1(const set<string>& terminals, const set<string>& nonterminals) {
		set<int> dupStates;  //��ͻ״̬����
		const int width = 10;

		cout << "   ";

		for (string terminal : terminals) {
			cout << setw(width) << setfill(' ') << setiosflags(ios::left) << terminal;
		}

		for (string nonterminal : nonterminals) {
			cout << setw(width) << setfill(' ') << setiosflags(ios::left) << nonterminal;
		}

		cout << endl;

		for (size_t i = 0; i < this->tableLR1.size(); i++) {
			cout << setw(4) << setfill(' ') << setiosflags(ios::left) << i;

			for (string terminal : terminals) {
				auto place = this->tableLR1[i].find(terminal);

				if (place != this->tableLR1[i].end()) {
					if (place->second.size() != 1)  //���������ϴ�С����1����״̬Ϊ��ͻ״̬�����м�¼
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
					if (place->second.size() != 1)  //���������ϴ�С����1����״̬Ϊ��ͻ״̬�����м�¼
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

	/***************************************************************************
	  �������ƣ�WriteTableLR1
	  ��    �ܣ���LR(1)�����������csv�����
	  ���������ofstream& out��csv�ļ�
				const set<string>& terminals���ս�����ϣ�ֻ����
				const set<string>& nonterminals�����ս�����ϣ�ֻ����
	  �� �� ֵ����
	  ˵    ����������ͬʱд���˳�ͻ״̬�������д��Ӧ�����ܿس�����η����ȡ
	***************************************************************************/
	void DFA::WriteTableLR1(ofstream& out, const set<string>& terminals, const set<string>& nonterminals) {
		set<int> dupStates;  //��ͻ״̬����
		const int width = 10;

		out << "id,";

		for (string terminal : terminals) {
			if (terminal == "EPSILON")
				continue;

			out << terminal << ',';
		}

		for (string nonterminal : nonterminals) {
			out << nonterminal << ',';
		}

		out << endl;

		for (size_t i = 0; i < this->tableLR1.size(); i++) {
			out << i << ',';

			for (string terminal : terminals) {
				if (terminal == "EPSILON")
					continue;

				auto place = this->tableLR1[i].find(terminal);

				if (place != this->tableLR1[i].end()) {
					if (place->second.size() != 1)  //���������ϴ�С����1����״̬Ϊ��ͻ״̬�����м�¼
						dupStates.insert(i);

					out << this->FormatTrans(place->second) << ',';
				}
				else {
					out << ',';
				}
			}

			for (string nonterminal : nonterminals) {
				auto place = this->tableLR1[i].find(nonterminal);

				if (place != this->tableLR1[i].end()) {
					if (place->second.size() != 1)  //���������ϴ�С����1����״̬Ϊ��ͻ״̬�����м�¼
						dupStates.insert(i);

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

	/***************************************************************************
	  �������ƣ�ReadWords
	  ��    �ܣ���һ�ݴʷ������������
	  ���������const char* filename���ļ�����ֻ����
	  �� �� ֵ��vector<string>�������б�
	  ˵    ������Ӧ���֣�Ӧ����ֲ���ܿس�����
	***************************************************************************/
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

	/***************************************************************************
	  �������ƣ�GrammarAnalysis
	  ��    �ܣ�ģ���ܿس����Դ��������﷨���������������̨��
	  ���������vector<string>& input��Դ����ʷ������������
	  �� �� ֵ��bool���﷨�����Ƿ�ͨ��
	  ˵    ������Ӧ���֣�Ӧ����ֲ���ܿس�����
	***************************************************************************/
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