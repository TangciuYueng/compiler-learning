#include"DFA.h"

namespace automat {
	set<int> DFA::OriginNFATrans(const int init, const char ch)const {
		int next = this->originNFATransList->getFirstNeighbor(init);
		set<int> ret;

		while (next != -1) {
			string rule;
			this->originNFATransList->getEdge(rule, init, next);

			if (rule == "<letter>") {
				if (isalpha(ch))
					ret.insert(next);
			}
			else if (rule == "<digit>") {
				if (isdigit(ch))
					ret.insert(next);
			}
			else if (rule != "<epsilon>") {
				if (rule.find(ch) != string::npos)
					ret.insert(next);
			}

			next = this->originNFATransList->getNextNeighbor(init, next);
		}

		return ret;
	}

	set<int> DFA::GetClosure(const set<int>& origin)const {
		set<int> closure;
		queue<int> openlist;
		set<int> closelist;
		int u, v, uPos, vPos;

		for (int state : origin) {
			openlist.push(state);
			closure.insert(state);
		}

		while (!openlist.empty()) {
			u = openlist.front();
			openlist.pop();
			uPos = this->originNFATransList->getVertexPos(u);
			vPos = this->originNFATransList->getFirstNeighbor(uPos);

			while (vPos != -1) {
				string edge;
				this->originNFATransList->getEdge(edge, uPos, vPos);

				if (edge == "<epsilon>") {
					this->originNFATransList->getVertex(v, vPos);
					closure.insert(v);

					if (closelist.find(v) == closelist.end())
						openlist.push(v);
				}

				vPos = this->originNFATransList->getNextNeighbor(uPos, vPos);
			}

			closelist.insert(u);
		}

		return closure;
	}

	void DFA::SubsetMethod() {
		map<set<int>, int> recordStates;  //�ѳ��ֵ�״̬��¼
		queue<NormState> remainStates;    //��������״̬
		set<int> initState;
		initState.insert(this->originNFAStart);
		initState = this->GetClosure(initState);   //ֻ��ԭNFA��Ψһ��ʼ״̬��հ�

		recordStates.emplace(initState, 0);
		remainStates.push(NormState(initState, 0));  //��ΪDFA����ʼ״̬����¼���������������
		this->detailedStateMap.emplace(0, initState);

		while (!remainStates.empty()) {
			vector<int> line;
			NormState state = remainStates.front();
			remainStates.pop();
			line.push_back(state.num);

			for (char token : tokens)  //��ÿ�����ܳ��ֵĺϷ��ַ����б���
			{
				set<int> transedState;  //�������״̬��

				for (int stat : state.state)
				{
					for (int s : this->OriginNFATrans(stat, token)) {
						transedState.insert(s);
					}
				}

				transedState = this->GetClosure(transedState);

				//����״̬δ���ֹ�
				if (recordStates.find(transedState) == recordStates.end()) {
					//Ϊ�ռ�
					if (transedState.size() == 0) {
						line.push_back(-2);
					}
					//���ǿռ�
					else {
						const int num = recordStates.size();
						remainStates.push(NormState(transedState, num));
						recordStates.emplace(transedState, num);
						line.push_back(num);

						this->detailedStateMap.emplace(num, transedState);  //��¼״̬��ϸ��Ϣ

						//�ж��Ƿ�Ϊʼ̬
						if (transedState.find(this->originNFAStart) != transedState.end())
							this->startList.insert(num);
						//�ж��Ƿ�Ϊ��̬
						else {
							set<int> inter;
							set_intersection(transedState.begin(), transedState.end(),
								this->originNFAEndList->begin(), this->originNFAEndList->end(),
								inserter(inter, inter.begin()));

							//������Ϊ�գ�����̬���������ȼ���������
							if (inter.size() != 0) {
								this->endList.insert(num);
								int min = INT_MAX;
								string regular;

								for (int state : inter) {
									auto iter = this->originNFAEndInfoMap->find(state);
									NFAEndInfo info = (*iter).second;

									if (min > info.priority) {
										min = info.priority;
										regular = info.regular;
									}
								}

								this->endMap.emplace(num, regular);
							}

						}
						//this->transList.insertEdge(state.num, num, string() += token);
						//this->out << state.num << ' ' << num << ' ' << token << '\n';
						//printf("��ӱߣ�%d %d %c\n", state.num, num, token);
					}
				}
				else {
					const int num = recordStates[transedState];
					line.push_back(num);
					//this->transList.insertEdge(state.num, num, string() += token);
					//this->out << state.num << ' ' << num << ' ' << token << '\n';
					//printf("��ӱߣ�%d %d %c\n", state.num, num, token);
				}
			}

			this->stateTransTable.push_back(line);
		}
	}

	void DFA::ShowStateTransTable() {
		cout << this->stateTransTable.size() << endl;

		for (vector<int> line : this->stateTransTable) {
			for (int state : line) {
				cout << state << ' ';
			}

			cout << "$$$$$" << endl;
		}
		//cout << this->transList;

		for (int state : this->startList) {
			cout << state << ' ';
		}
		cout << endl;
		for (int state : this->endList) {
			cout << "״̬��" << state << " ʶ��" << this->endMap[state] << endl;
		}
		for (auto pair : this->detailedStateMap) {
			cout << "״̬��" << pair.first << " ����NFA״̬��";

			for (int state : pair.second) {
				cout << state << ' ';
			}

			cout << endl;
		}
	}

	void DFA::WriteStateTransTable(ofstream& out) {
		if (!out) {
			cout << "�ļ���ʧ�ܣ�" << endl;
			return;
		}

		for (size_t i = 0; i < this->stateTransTable.size(); i++)
		{
			for (size_t j = 1; j < this->stateTransTable[i].size(); j++)
			{
				out << i << "--" << this->tokens[j - 1] << "-->" << this->stateTransTable[i][j] << endl;
			}
		}
	}

	void DFA::DetailedSubsetMethod() {
		set<set<int>> recordStates;
		queue<set<int>> remainStates;
		set<int> start;
		start.insert(this->originNFAStart);
		set<int> initState = this->GetClosure(start);
		recordStates.insert(initState);
		remainStates.push(initState);

		while (!remainStates.empty()) {
			vector<set<int>> line;
			set<int> state = remainStates.front();
			remainStates.pop();
			line.push_back(state);

			for (char token : tokens)
			{
				set<int> transedState;

				for (int stat : state)
				{
					set<int> singleTransedState = this->OriginNFATrans(stat, token);

					for (int s : singleTransedState) {
						transedState.insert(s);
					}
				}

				transedState = this->GetClosure(transedState);
				line.push_back(transedState);

				if (recordStates.find(transedState) == recordStates.end()) {
					remainStates.push(transedState);
					recordStates.insert(transedState);
				}
			}

			this->detailedStateTransTable.push_back(line);
		}
	}

	void DFA::ShowDetailedStateTransTable()const {
		cout << this->detailedStateTransTable.size() << endl;
		for (vector<set<int>> line : this->detailedStateTransTable) {
			for (set<int> state : line) {
				for (int s : state) {
					cout << s << ' ';
				}

				cout << '|';
			}

			cout << endl;
		}
	}

	bool DFA::LexicalAnalysis(string input) {
		stack<int> stack;
		string::iterator iter = input.begin();
		int current = 0;
		string word;
		cout << setiosflags(ios::left);

		while (iter != input.end()) {
			//��ȡһ���ַ��Ĺ���
			if (*iter == ' ' || *iter == '\n' || *iter == '\r') {
				if (stack.size() == 0) {
					cout << "throw LexicalAnalysisError" << endl;
					return false;
				}

				while (this->endList.find(current) == this->endList.end()) {
					if (stack.size() == 0) {
						cout << "throw LexicalAnalysisError" << endl;
						return false;
					}

					current = stack.top();
					stack.pop();
					iter--;
					word.erase(word.length() - 1);
					//cout << "�ص���" << current << endl;
				}

				cout << "�ʣ�" << setw(10) << word << "         ƥ�䣺" << this->endMap[current] << endl;
				word.clear();
				current = 0;
			}


			else {
				//�Ƿ��ַ�������
				if (tokens.find(*iter) == string::npos) {
					cout << "throw LexIllegalTokenError" << endl;
					return false;
				}

				const int next = this->stateTransTable[current][tokens.find(*iter) + 1];

				if (next == -2) {
					//���ݡ����վ����򱨴�
					if (stack.size() == 0) {
						cout << "throw LexicalAnalysisError" << endl;
						return false;
					}

					while (this->endList.find(current) == this->endList.end()) {
						if (stack.size() == 0) {
							cout << "throw LexicalAnalysisError" << endl;
							return false;
						}

						current = stack.top();
						stack.pop();
						iter--;
						word.erase(word.length() - 1);
						//cout << "�ص���" << current << endl;
					}

					cout << "�ʣ�" << word << " ƥ�䣺" << this->endMap[current] << endl;
					word.clear();
					current = 0;
					iter--;
				}

				else {
					//cout << "Ǩ�ƣ�" << current << "-->" << next << endl;
					current = next;
					word += *iter;
					stack.push(current);
				}
			}

			iter++;
		}

		return true;
	}
}