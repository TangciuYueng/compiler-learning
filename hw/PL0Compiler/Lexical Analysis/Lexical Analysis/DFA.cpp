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
		map<set<int>, int> recordStates;  //已出现的状态记录
		queue<NormState> remainStates;    //待求结果的状态
		set<int> initState;
		initState.insert(this->originNFAStart);
		initState = this->GetClosure(initState);   //只将原NFA的唯一起始状态求闭包

		recordStates.emplace(initState, 0);
		remainStates.push(NormState(initState, 0));  //作为DFA的起始状态，记录、放入待求结果序列
		this->detailedStateMap.emplace(0, initState);

		while (!remainStates.empty()) {
			vector<int> line;
			NormState state = remainStates.front();
			remainStates.pop();
			line.push_back(state.num);

			for (char token : tokens)  //对每个可能出现的合法字符进行遍历
			{
				set<int> transedState;  //遍历后的状态集

				for (int stat : state.state)
				{
					for (int s : this->OriginNFATrans(stat, token)) {
						transedState.insert(s);
					}
				}

				transedState = this->GetClosure(transedState);

				//若该状态未出现过
				if (recordStates.find(transedState) == recordStates.end()) {
					//为空集
					if (transedState.size() == 0) {
						line.push_back(-2);
					}
					//不是空集
					else {
						const int num = recordStates.size();
						remainStates.push(NormState(transedState, num));
						recordStates.emplace(transedState, num);
						line.push_back(num);

						this->detailedStateMap.emplace(num, transedState);  //记录状态详细信息

						//判断是否为始态
						if (transedState.find(this->originNFAStart) != transedState.end())
							this->startList.insert(num);
						//判断是否为终态
						else {
							set<int> inter;
							set_intersection(transedState.begin(), transedState.end(),
								this->originNFAEndList->begin(), this->originNFAEndList->end(),
								inserter(inter, inter.begin()));

							//交集不为空，是终态，根据优先级决定产出
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
						//printf("添加边：%d %d %c\n", state.num, num, token);
					}
				}
				else {
					const int num = recordStates[transedState];
					line.push_back(num);
					//this->transList.insertEdge(state.num, num, string() += token);
					//this->out << state.num << ' ' << num << ' ' << token << '\n';
					//printf("添加边：%d %d %c\n", state.num, num, token);
				}
			}

			this->stateTransTable.push_back(line);
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
				map<int, set<int>> divideMap;  //当前状态集之中，每个状态接收某个字符后落入的状态集号和落入该集的所有状态集合
				
				for (int state : *iter) {
					const int next = this->stateTransTable[state][tokens.find(ch) + 1];  //得到迁移后状态

					//寻找落入集合的下标
					vector<set<int>>::iterator divide = divides.begin();
					while (divide != divides.end()) {
						if (divide->find(next) != divide->end()) {  //落入集合找到
							const int index = divide - divides.begin();  //得到下标

							//检查divideMap有无index为键的对，有则添加，无则添加
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

					if (divide == divides.end()) {  //无法落入，说明该状态无法接收当前字符

					}
				}

				//检查divideMap的大小，若大于1则需要分解
				if (divideMap.size() > 1) {
					cout << ch << "可以区分" << endl;
					divides.erase(iter);     //删除原有的状态集

					for (auto pair : divideMap) {  //将分解后的结果加入divides
						divides.push_back(pair.second);
					}
					
					iter = divides.begin();  //回到原点，重新检查
					ifDivided = true;        //置标志为真
					break;                   //当前字符已经可以区分，跳出该层循环
				}
				//等于1说明不需分解，检查下一字符能否区分
				else {
					cout << ch << "无法区分" << endl;
				}
			}
			
			if (!ifDivided) {  //若当前状态集没有分开，则检查下一状态集
				iter++;
			}
		}

		for (set<int> divide : divides) {
			cout << "分隔后状态：";

			for (int state : divide) {
				cout << state << ' ';
			}

			cout << endl;
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
			cout << "状态：" << state << " 识别：" << this->endMap[state] << endl;
		}
		for (auto pair : this->detailedStateMap) {
			cout << "状态：" << pair.first << " 包含NFA状态：";

			for (int state : pair.second) {
				cout << state << ' ';
			}

			cout << endl;
		}
	}

	void DFA::WriteStateTransTable(ofstream& out) {
		if (!out) {
			cout << "文件打开失败！" << endl;
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
			//获取一个字符的过程
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
					//cout << "回到：" << current << endl;
				}

				cout << "词：" << setw(10) << word << "         匹配：" << this->endMap[current] << endl;
				word.clear();
				current = 0;
			}


			else {
				//非法字符，报错
				if (tokens.find(*iter) == string::npos) {
					cout << "throw LexIllegalTokenError" << endl;
					return false;
				}

				const int next = this->stateTransTable[current][tokens.find(*iter) + 1];

				if (next == -2) {
					//回溯、最终决定或报错
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
						//cout << "回到：" << current << endl;
					}

					cout << "词：" << word << " 匹配：" << this->endMap[current] << endl;
					word.clear();
					current = 0;
					iter--;
				}

				else {
					//cout << "迁移：" << current << "-->" << next << endl;
					current = next;
					word += *iter;
					stack.push(current);
				}
			}

			iter++;
		}

		return true;
	}

	bool DFA::WriteLexicalAnalysis(string input, ofstream& out, map<string, string>& regularDescriptionMap) {
		stack<int> stack;
		string::iterator iter = input.begin();
		int current = 0;
		string word;

		while (iter != input.end()) {
			//获取一个字符的过程
			if (*iter == ' ' || *iter == '\n' || *iter == '\r') {
				if (stack.size() == 0) {
					out << "throw LexicalAnalysisError" << endl;
					return false;
				}

				while (this->endList.find(current) == this->endList.end()) {
					if (stack.size() == 0) {
						out << "throw LexicalAnalysisError" << endl;
						return false;
					}

					current = stack.top();
					stack.pop();
					iter--;
					word.erase(word.length() - 1);
					//cout << "回到：" << current << endl;
				}

				out << regularDescriptionMap.find(this->endMap[current])->second << endl;
				if (word == "WHILE" || word == "DO" || word == "THEN")
					out << "M" << endl;
				word.clear();
				current = 0;
			}


			else {
				//非法字符，报错
				if (tokens.find(*iter) == string::npos) {
					out << "throw LexIllegalTokenError" << endl;
					return false;
				}

				const int next = this->stateTransTable[current][tokens.find(*iter) + 1];

				if (next == -2) {
					//回溯、最终决定或报错
					if (stack.size() == 0) {
						out << "throw LexicalAnalysisError" << endl;
						return false;
					}

					while (this->endList.find(current) == this->endList.end()) {
						if (stack.size() == 0) {
							out << "throw LexicalAnalysisError" << endl;
							return false;
						}

						current = stack.top();
						stack.pop();
						iter--;
						word.erase(word.length() - 1);
						//cout << "回到：" << current << endl;
					}

					out << regularDescriptionMap.find(this->endMap[current])->second << endl;
					if (word == "WHILE" || word == "DO" || word == "THEN")
						out << "M" << endl;
					word.clear();
					current = 0;
					iter--;
				}

				else {
					//cout << "迁移：" << current << "-->" << next << endl;
					current = next;
					word += *iter;
					stack.push(current);
				}
			}

			iter++;
		}

		out << "TERMINAL" << endl;  //添加起始符，方便语法分析
		return true;
	}
}