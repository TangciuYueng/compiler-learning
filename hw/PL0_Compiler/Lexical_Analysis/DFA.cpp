#include"DFA.h"

namespace automat {
	/***************************************************************************
	  �������ƣ�OriginNFATrans
	  ��    �ܣ�����ԭNFA��һ��״̬���������ĳ�ַ�����ת������״̬����
	  ���������const int init����ʼ״̬��ֻ����
	            const char ch�����յ��ַ���ֻ����
	  �� �� ֵ��set<int>����ת������״̬����
	  ˵    �����ú��������Ӽ������㣬�ʲ�����տ���
	***************************************************************************/
	set<int> DFA::OriginNFATrans(const int init, const char ch)const {
		const int initPos = this->originNFATransList->getVertexPos(init);
		int nextPos = this->originNFATransList->getFirstNeighbor(initPos);
		int next;
		this->originNFATransList->getVertex(next, nextPos);
		set<int> ret;

		while (nextPos != -1) {  //������״̬�������ھӣ����ݱ�ֵ����ת��
			string rule;
			this->originNFATransList->getEdge(rule, initPos, nextPos);

			if (rule == "<letter>") {  //����������ĸ
				if (isalpha(ch)) {
					this->originNFATransList->getVertex(next, nextPos);
					ret.insert(next);
				}
			}
			else if (rule == "<digit>") {  //������������
				if (isdigit(ch)) {
					this->originNFATransList->getVertex(next, nextPos);
					ret.insert(next);
				}
			}
			else if (rule != "<epsilon>") {  //�����տ���
				if (rule.find(ch) != string::npos) {
					this->originNFATransList->getVertex(next, nextPos);
					ret.insert(next);
				}
			}

			nextPos = this->originNFATransList->getNextNeighbor(initPos, nextPos);
		}

		return ret;
	}

	/***************************************************************************
	  �������ƣ�GetClosure
	  ��    �ܣ�����ԭNFA��һ��״̬���ϣ������հ�
	  ���������const set<int>& origin��״̬���ϣ�ֻ����
	  �� �� ֵ��set<int>���հ�
	  ˵    ����closelist��ֹ�б�ȫΪ���ֵĻ�����
	***************************************************************************/
	set<int> DFA::GetClosure(const set<int>& origin)const {
		set<int> closure;
		queue<int> openlist;
		set<int> closelist;
		int u, v, uPos, vPos;

		for (int state : origin) {  //�հ����Ȱ����Լ����ҷ���openlist��
			openlist.push(state);
			closure.insert(state);
		}

		while (!openlist.empty()) {  //���openlist�еĽڵ㣬�о���һ���Ż�������ھ�����뷵�صļ��ϣ������ýڵ����closelist
			u = openlist.front();
			openlist.pop();
			uPos = this->originNFATransList->getVertexPos(u);
			vPos = this->originNFATransList->getFirstNeighbor(uPos);

			while (vPos != -1) {
				string edge;
				this->originNFATransList->getEdge(edge, uPos, vPos);

				if (edge == "<epsilon>") {  //�о���һ���Ż�������ھ�����뷵�صļ���
					this->originNFATransList->getVertex(v, vPos);
					closure.insert(v);

					if (closelist.find(v) == closelist.end())  //���ھ�������closelist�У������openlist�����
						openlist.push(v);
				}

				vPos = this->originNFATransList->getNextNeighbor(uPos, vPos);
			}

			closelist.insert(u);  //����ǰ�ڵ����closelist��
		}

		return closure;
	}

	/***************************************************************************
	  �������ƣ�SubsetMethod
	  ��    �ܣ����Ӽ�����NFAת��ΪDFA
	  �����������
	  �� �� ֵ����
	  ˵    ����
	***************************************************************************/
	void DFA::SubsetMethod() {
		map<set<int>, int> recordStates;  //�ѳ��ֵ�״̬��¼
		queue<DFAState> remainStates;    //��������״̬
		set<int> initState;
		initState.insert(this->originNFAStart);
		initState = this->GetClosure(initState);   //ֻ��ԭNFA��Ψһ��ʼ״̬��հ�

		this->startList.insert(0);                   //0��״̬��ȻΪDFA��ʼ̬
		recordStates.emplace(initState, 0);
		remainStates.push(DFAState(initState, 0));  //��ΪDFA����ʼ״̬����¼���������������
		this->detailedStateMap.emplace(0, initState);

		while (!remainStates.empty()) {  //��������״̬û��飬��Ӷ�����ȡ����������
			vector<int> line;                        //�Ӽ�״̬���µ�һ��
			DFAState state = remainStates.front();  //����״̬����
			remainStates.pop();
			line.push_back(state.num);               //��״̬�ı������һ�е���ʼ

			for (char token : tokens)  //��ÿ�����ܳ��ֵĺϷ��ַ����б���
			{
				set<int> transedState;  //�������״̬��

				for (int stat : state.state)  //��ԭ״̬����ÿһ��״̬����ͳ�ƽ��ո��ַ�ת�����״̬���ϣ��ٶ�����հ�
				{
					for (int s : this->OriginNFATrans(stat, token)) {
						transedState.insert(s);
					}
				}

				transedState = this->GetClosure(transedState);

				//����״̬δ���ֹ�
				if (recordStates.find(transedState) == recordStates.end()) {
					//Ϊ�ռ���˵����ǰ״̬���޷����ո��ַ���״̬ת�����м���Ƿ�ֵ
					if (transedState.size() == 0) {
						line.push_back(-2);
					}
					//���ǿռ�
					else {
						const int num = recordStates.size();  //��0��ʼ��ţ���״̬�ĺ�����recordStates�Ĵ�С
						remainStates.push(DFAState(transedState, num));  //�����������
						recordStates.emplace(transedState, num);          //�����ѳ���״̬���ļ���
						line.push_back(num);                              //�����µ�һ��

						this->detailedStateMap.emplace(num, transedState);  //��¼״̬��ϸ��Ϣ

						//�ж��Ƿ�ΪDFAʼ̬
						if (transedState.find(this->originNFAStart) != transedState.end())
							this->startList.insert(num);
						//�ж��Ƿ�ΪDFA��̬
						else {
							set<int> inter;  //��������״̬����ԭNFA��̬���ϵĽ���
							set_intersection(transedState.begin(), transedState.end(),
								this->originNFAEndList->begin(), this->originNFAEndList->end(),
								inserter(inter, inter.begin()));

							//������Ϊ�գ�����̬
							if (inter.size() != 0) {
								this->endList.insert(num);
								int min = INT_MAX;
								string regular;

								for (int state : inter) {  //ȡ������������ȼ���ߵ���̬��NFA����ʶ�������ʽ����Ϊ��DFA��̬��ʶ����
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
				//����״̬���ֹ�����鵽���ı�Ų������µ�һ��
				else {
					const int num = recordStates[transedState];
					line.push_back(num);
					//this->transList.insertEdge(state.num, num, string() += token);
					//this->out << state.num << ' ' << num << ' ' << token << '\n';
					//printf("��ӱߣ�%d %d %c\n", state.num, num, token);
				}
			}

			this->stateTransTable.push_back(line);  //������ȫ���ַ��󣬽��µ�һ�м�����״̬ת����
		}
	}

	void DFA::Simplify() {
		vector<set<int>> divides;
		set<int> all, start;

		for (size_t i = 0; i < this->stateTransTable.size(); i++)
		{
			all.insert(i);
		}

		set_difference(all.begin(), all.end(), this->endList.begin(), this->endList.end(), std::inserter(start, start.begin()));
		divides.push_back(start);
		divides.push_back(this->endList);
		
		vector<set<int>>::iterator iter = divides.begin();

		while (iter != divides.end()) {
			if (iter->size() == 1) {
				iter++;
				continue;
			}

			bool ifDivided = false;

			for (char ch : this->tokens) {
				map<int, set<int>> divideMap;  //��ǰ״̬��֮�У�ÿ��״̬����ĳ���ַ��������״̬���ź�����ü�������״̬����
				
				for (int state : *iter) {
					const int next = this->stateTransTable[state][tokens.find(ch) + 1];  //�õ�Ǩ�ƺ�״̬

					//Ѱ�����뼯�ϵ��±�
					vector<set<int>>::iterator divide = divides.begin();
					while (divide != divides.end()) {
						if (divide->find(next) != divide->end()) {  //���뼯���ҵ�
							const int index = divide - divides.begin();  //�õ��±�

							//���divideMap����indexΪ���Ķԣ�������ӣ��������
							map<int, set<int>>::iterator pair = divideMap.find(index);
							if (pair != divideMap.end()) {
								pair->second.insert(state);
							}
							else {
								set<int> newSet = { state };
								divideMap.emplace(index, newSet);
							}
						}

						divide++;
					}

					if (divide == divides.end()) {  //�޷����룬˵����״̬�޷����յ�ǰ�ַ�

					}
				}

				//���divideMap�Ĵ�С��������1����Ҫ�ֽ�
				if (divideMap.size() > 1) {
					cout << ch << "��������" << endl;
					divides.erase(iter);     //ɾ��ԭ�е�״̬��

					for (auto pair : divideMap) {  //���ֽ��Ľ������divides
						divides.push_back(pair.second);
					}
					
					iter = divides.begin();  //�ص�ԭ�㣬���¼��
					ifDivided = true;        //�ñ�־Ϊ��
					break;                   //��ǰ�ַ��Ѿ��������֣������ò�ѭ��
				}
				//����1˵������ֽ⣬�����һ�ַ��ܷ�����
				else {
					cout << ch << "�޷�����" << endl;
				}
			}
			
			if (!ifDivided) {  //����ǰ״̬��û�зֿ���������һ״̬��
				iter++;
			}
		}

		for (set<int> divide : divides) {
			cout << "�ָ���״̬��";

			for (int state : divide) {
				cout << state << ' ';
			}

			cout << endl;
		}
	}

	/***************************************************************************
	  �������ƣ�ShowStateTransTable
	  ��    �ܣ���ʽ�����״̬ת������
	  �����������
	  �� �� ֵ����
	  ˵    �������Ϊ-2����ĳ��״̬���޷�����ĳ���ַ���ʱ����ո�
	***************************************************************************/
	void DFA::ShowStateTransTable() {
		cout << "    ";
		for (char token : this->tokens) {
			cout << setw(4) << setfill(' ') << setiosflags(ios::left) << token;
		}
		cout << endl;

		for (vector<int> line : this->stateTransTable) {
			for (int state : line) {
				if (state != -2)
					cout << setw(4) << setfill(' ') << setiosflags(ios::left) << state;
				else
					cout << "    ";
			}

			cout << endl;
		}
	}

	/***************************************************************************
	  �������ƣ�WriteStateTransTable
	  ��    �ܣ���DFAд���ļ����ܿس����ȡ
	  ���������ofstream& out��������ļ���
	  �� �� ֵ����
	  ˵    �������ܿس������д�뷽ʽ��δ���
	***************************************************************************/
	void DFA::WriteStateTransTable(ofstream& out) {
		if (!out) {
			cout << "�ļ���ʧ�ܣ�" << endl;
			return;
		}

		out << ',';
		for (char token : this->tokens) {
			if (token != ',')
				out << token << ',';
			else
				out << "��" << ',';
		}
		out << endl;

		for (vector<int> line : this->stateTransTable) {
			for (int state : line) {
				if (state != -2)
					out << state << ',';
				else
					out << ',';
			}

			out << endl;
		}
	}

	/***************************************************************************
	  �������ƣ�LexicalAnalysis
	  ��    �ܣ�ģ���ܿس����Դ������дʷ����������������̨��
	  ���������string input��Դ��������
				map<string, string>& regularDescriptionMap������ʽ���﷨�����ս����ӳ���
	  �� �� ֵ��bool���ʷ������Ƿ�ͨ��
	  ˵    ������Ӧ���֣�Ӧ����ֲ���ܿس�����
	***************************************************************************/
	bool DFA::LexicalAnalysis(string input, map<string, string>& regularDescriptionMap) {
		stack<int> stack;
		string::iterator iter = input.begin();
		int current = 0;
		string word;

		while (iter != input.end()) {
			if (*iter == ' ' || *iter == '\n' || *iter == '\r') {  //��������ָ������з���ǿ�Ƴɴ�
				if (stack.size() == 0) {  //��������ķָ������֣������
					iter++;
					continue;
				}

				//����ʲ�Ϊ�գ���ӵ�ǰDFA״̬���ݣ��ҵ�һ����̬��ֹͣ����ѯ���״̬ʶ��Ĵ����Ͳ���ӡ
				while (this->endList.find(current) == this->endList.end()) {
					if (stack.size() == 0) {  //���˵�ջ����Ȼû�ҵ���̬���򱨴�
						cout << "throw LexicalAnalysisError" << endl;
						return false;
					}

					current = stack.top();
					stack.pop();
					iter--;
					word.erase(word.length() - 1);
				}

				cout << setiosflags(ios::left) << setw(20) << setfill(' ') << "�ʣ�" + word
					<< setiosflags(ios::left) << setw(20) << setfill(' ') << "ƥ�䣺" + this->endMap[current] << endl;
				
				//���word��ջ��״̬�ص���̬
				word.clear();
				while (!stack.empty()) {
					stack.pop();
				}
				current = 0;
			}

			else {
				//�Ƿ��ַ�������
				if (tokens.find(*iter) == string::npos) {
					cout << "throw LexIllegalTokenError" << endl;
					return false;
				}

				const int next = this->stateTransTable[current][tokens.find(*iter) + 1];  //��״̬ת�ƾ���õ���һ״̬

				//���ܽ��ո��ַ�
				if (next == -2) {
					//����Ϊ��
					if (stack.size() == 0) {
						cout << "throw LexicalAnalysisError" << endl;
						return false;
					}

					//����ʲ�Ϊ�գ���ӵ�ǰDFA״̬���ݣ��ҵ�һ����̬��ֹͣ����ѯ���״̬ʶ��Ĵ����Ͳ���ӡ
					while (this->endList.find(current) == this->endList.end()) {
						if (stack.size() == 0) {
							cout << "throw LexicalAnalysisError" << endl;
							return false;
						}

						current = stack.top();
						stack.pop();
						iter--;
						word.erase(word.length() - 1);
					}

					cout << setiosflags(ios::left) << setw(20) << setfill(' ') << "�ʣ�" + word
						<< setiosflags(ios::left) << setw(20) << setfill(' ') << "ƥ�䣺" + this->endMap[current] << endl;
					
					//���word��ջ��״̬�ص���̬��ͬʱ��ǰ�ַ�û�н��������ɵĴ��ڣ�����������һ����λ
					word.clear();
					while (!stack.empty()) {
						stack.pop();
					}
					current = 0;
					iter--;
				}

				//�ܽ��ո��ַ������ܸ��ַ�������״̬Ǩ��
				else {
					current = next;
					word += *iter;
					stack.push(current);
				}
			}

			iter++;
		}

		return true;
	}

	/***************************************************************************
	  �������ƣ�WriteLexicalAnalysis
	  ��    �ܣ�ģ���ܿس����Դ������дʷ�������������ļ���
	  ���������string input��Դ��������
				ofstream& out��������ļ���
				map<string, string>& regularDescriptionMap������ʽ���﷨�����ս����ӳ���
	  �� �� ֵ��bool���ʷ������Ƿ�ͨ��
	  ˵    ����Ӧ�������ܿس��򷽱��ȡ�ķ�ʽд��
	***************************************************************************/
	bool DFA::WriteLexicalAnalysis(string input, ofstream& out, map<string, string>& regularDescriptionMap) {
		stack<int> stack;
		string::iterator iter = input.begin();
		int current = 0;
		string word;

		while (iter != input.end()) {
			//��ȡһ���ַ��Ĺ���
			if (*iter == ' ' || *iter == '\n' || *iter == '\r') {
				if (stack.size() == 0) {  //��������ķָ������֣������
					iter++;
					continue;
				}

				//����ʲ�Ϊ�գ���ӵ�ǰDFA״̬���ݣ��ҵ�һ����̬��ֹͣ����ѯ���״̬ʶ��Ĵ����Ͳ���ӡ
				while (this->endList.find(current) == this->endList.end()) {
					if (stack.size() == 0) {  //���˵�ջ����Ȼû�ҵ���̬���򱨴�
						cout << "throw LexicalAnalysisError" << endl;
						return false;
					}

					current = stack.top();
					stack.pop();
					iter--;
					word.erase(word.length() - 1);
				}

				out << regularDescriptionMap.find(this->endMap[current])->second << endl;
				if (word == "WHILE" || word == "DO" || word == "THEN")
					out << "M" << endl;
				//���word��ջ��״̬�ص���̬
				word.clear();
				while (!stack.empty()) {
					stack.pop();
				}
				current = 0;
			}


			else {
				//�Ƿ��ַ�������
				if (tokens.find(*iter) == string::npos) {
					out << "throw LexIllegalTokenError" << endl;
					return false;
				}

				const int next = this->stateTransTable[current][tokens.find(*iter) + 1];

				//���ܽ��ո��ַ�
				if (next == -2) {
					//����Ϊ��
					if (stack.size() == 0) {
						out << "throw LexicalAnalysisError" << endl;
						return false;
					}

					//����ʲ�Ϊ�գ���ӵ�ǰDFA״̬���ݣ��ҵ�һ����̬��ֹͣ����ѯ���״̬ʶ��Ĵ����Ͳ���ӡ
					while (this->endList.find(current) == this->endList.end()) {
						if (stack.size() == 0) {
							out << "throw LexicalAnalysisError" << endl;
							return false;
						}

						current = stack.top();
						stack.pop();
						iter--;
						word.erase(word.length() - 1);
					}

					out << regularDescriptionMap.find(this->endMap[current])->second << endl;
					if (word == "WHILE" || word == "DO" || word == "THEN")
						out << "M" << endl;
					//���word��ջ��״̬�ص���̬��ͬʱ��ǰ�ַ�û�н��������ɵĴ��ڣ�����������һ����λ
					word.clear();
					while (!stack.empty()) {
						stack.pop();
					}
					current = 0;
					iter--;
				}

				//�ܽ��ո��ַ������ܸ��ַ�������״̬Ǩ��
				else {
					current = next;
					word += *iter;
					stack.push(current);
				}
			}

			iter++;
		}

		out << "TERMINAL" << endl;  //�����ʼ���������﷨����
		return true;
	}

	void DFA::PrintEndMap()const {
		auto iter = this->endMap.begin();

		while (iter != this->endMap.end()) {
			cout << iter->first << " --> " << iter->second << endl;
			iter++;
		}
	}

	void DFA::PrintEndList()const {
		auto iter = this->endList.begin();

		while (iter != this->endList.end()) {
			cout << *iter << ' ';
			iter++;
		}

		cout << endl;
	}
}