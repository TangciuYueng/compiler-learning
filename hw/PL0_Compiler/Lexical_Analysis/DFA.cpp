#include"DFA.h"

namespace automat {
	/***************************************************************************
	  函数名称：OriginNFATrans
	  功    能：对于原NFA的一个状态，输出接收某字符后能转换到的状态集合
	  输入参数：const int init：初始状态（只读）
	            const char ch：接收的字符（只读）
	  返 回 值：set<int>：能转换到的状态集合
	  说    明：该函数用于子集法计算，故不会接收空字
	***************************************************************************/
	set<int> DFA::OriginNFATrans(const int init, const char ch)const {
		const int initPos = this->originNFATransList->getVertexPos(init);
		int nextPos = this->originNFATransList->getFirstNeighbor(initPos);
		int next;
		this->originNFATransList->getVertex(next, nextPos);
		set<int> ret;

		while (nextPos != -1) {  //遍历该状态的所有邻居，根据边值进行转换
			string rule;
			this->originNFATransList->getEdge(rule, initPos, nextPos);

			if (rule == "<letter>") {  //接收所有字母
				if (isalpha(ch)) {
					this->originNFATransList->getVertex(next, nextPos);
					ret.insert(next);
				}
			}
			else if (rule == "<digit>") {  //接收所有数字
				if (isdigit(ch)) {
					this->originNFATransList->getVertex(next, nextPos);
					ret.insert(next);
				}
			}
			else if (rule != "<epsilon>") {  //不接收空字
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
	  函数名称：GetClosure
	  功    能：对于原NFA的一个状态集合，求出其闭包
	  输入参数：const set<int>& origin：状态集合（只读）
	  返 回 值：set<int>：闭包
	  说    明：closelist防止有边全为空字的环出现
	***************************************************************************/
	set<int> DFA::GetClosure(const set<int>& origin)const {
		set<int> closure;
		queue<int> openlist;
		set<int> closelist;
		int u, v, uPos, vPos;

		for (int state : origin) {  //闭包首先包含自己，且放入openlist中
			openlist.push(state);
			closure.insert(state);
		}

		while (!openlist.empty()) {  //检查openlist中的节点，有经过一条ε弧到达的邻居则加入返回的集合，并将该节点放入closelist
			u = openlist.front();
			openlist.pop();
			uPos = this->originNFATransList->getVertexPos(u);
			vPos = this->originNFATransList->getFirstNeighbor(uPos);

			while (vPos != -1) {
				string edge;
				this->originNFATransList->getEdge(edge, uPos, vPos);

				if (edge == "<epsilon>") {  //有经过一条ε弧到达的邻居则加入返回的集合
					this->originNFATransList->getVertex(v, vPos);
					closure.insert(v);

					if (closelist.find(v) == closelist.end())  //该邻居若不在closelist中，则加入openlist待检查
						openlist.push(v);
				}

				vPos = this->originNFATransList->getNextNeighbor(uPos, vPos);
			}

			closelist.insert(u);  //将当前节点加入closelist中
		}

		return closure;
	}

	/***************************************************************************
	  函数名称：SubsetMethod
	  功    能：用子集法将NFA转换为DFA
	  输入参数：无
	  返 回 值：无
	  说    明：
	***************************************************************************/
	void DFA::SubsetMethod() {
		map<set<int>, int> recordStates;  //已出现的状态记录
		queue<DFAState> remainStates;    //待求结果的状态
		set<int> initState;
		initState.insert(this->originNFAStart);
		initState = this->GetClosure(initState);   //只将原NFA的唯一起始状态求闭包

		this->startList.insert(0);                   //0号状态显然为DFA起始态
		recordStates.emplace(initState, 0);
		remainStates.push(DFAState(initState, 0));  //作为DFA的起始状态，记录、放入待求结果序列
		this->detailedStateMap.emplace(0, initState);

		while (!remainStates.empty()) {  //若还有新状态没检查，则从队列中取出进行运算
			vector<int> line;                        //子集状态表新的一行
			DFAState state = remainStates.front();  //将新状态出队
			remainStates.pop();
			line.push_back(state.num);               //该状态的编号是新一行的起始

			for (char token : tokens)  //对每个可能出现的合法字符进行遍历
			{
				set<int> transedState;  //遍历后的状态集

				for (int stat : state.state)  //对原状态集的每一个状态，先统计接收该字符转换后的状态集合，再对其求闭包
				{
					for (int s : this->OriginNFATrans(stat, token)) {
						transedState.insert(s);
					}
				}

				transedState = this->GetClosure(transedState);

				//若该状态未出现过
				if (recordStates.find(transedState) == recordStates.end()) {
					//为空集，说明当前状态集无法接收该字符，状态转换表中加入非法值
					if (transedState.size() == 0) {
						line.push_back(-2);
					}
					//不是空集
					else {
						const int num = recordStates.size();  //从0开始编号，新状态的号码是recordStates的大小
						remainStates.push(DFAState(transedState, num));  //加入待检查队列
						recordStates.emplace(transedState, num);          //加入已出现状态集的集合
						line.push_back(num);                              //加入新的一行

						this->detailedStateMap.emplace(num, transedState);  //记录状态详细信息

						//判断是否为DFA始态
						if (transedState.find(this->originNFAStart) != transedState.end())
							this->startList.insert(num);
						//判断是否为DFA终态
						else {
							set<int> inter;  //求出这个新状态集与原NFA终态集合的交集
							set_intersection(transedState.begin(), transedState.end(),
								this->originNFAEndList->begin(), this->originNFAEndList->end(),
								inserter(inter, inter.begin()));

							//交集不为空，是终态
							if (inter.size() != 0) {
								this->endList.insert(num);
								int min = INT_MAX;
								string regular;

								for (int state : inter) {  //取这个交集中优先级最高的终态在NFA中能识别的正规式，作为该DFA终态的识别结果
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
				//若该状态出现过，则查到它的编号并加入新的一行
				else {
					const int num = recordStates[transedState];
					line.push_back(num);
					//this->transList.insertEdge(state.num, num, string() += token);
					//this->out << state.num << ' ' << num << ' ' << token << '\n';
					//printf("添加边：%d %d %c\n", state.num, num, token);
				}
			}

			this->stateTransTable.push_back(line);  //遍历完全部字符后，将新的一行加入总状态转换表
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

	/***************************************************************************
	  函数名称：ShowStateTransTable
	  功    能：格式化输出状态转换矩阵
	  输入参数：无
	  返 回 值：无
	  说    明：编号为-2（即某个状态集无法接收某个字符）时输出空格
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
	  函数名称：WriteStateTransTable
	  功    能：将DFA写入文件供总控程序读取
	  输入参数：ofstream& out：输出的文件流
	  返 回 值：无
	  说    明：待总控程序决定写入方式，未完成
	***************************************************************************/
	void DFA::WriteStateTransTable(ofstream& out) {
		if (!out) {
			cout << "文件打开失败！" << endl;
			return;
		}

		out << ',';
		for (char token : this->tokens) {
			if (token != ',')
				out << token << ',';
			else
				out << "，" << ',';
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
	  函数名称：LexicalAnalysis
	  功    能：模拟总控程序对源代码进行词法分析（输出到控制台）
	  输入参数：string input：源代码输入
				map<string, string>& regularDescriptionMap：正规式和语法分析终结符的映射表
	  返 回 值：bool：词法分析是否通过
	  说    明：不应出现，应当移植到总控程序中
	***************************************************************************/
	bool DFA::LexicalAnalysis(string input, map<string, string>& regularDescriptionMap) {
		stack<int> stack;
		string::iterator iter = input.begin();
		int current = 0;
		string word;

		while (iter != input.end()) {
			if (*iter == ' ' || *iter == '\n' || *iter == '\r') {  //如果遇到分隔、换行符则强制成词
				if (stack.size() == 0) {  //多个连续的分隔符出现，则忽略
					iter++;
					continue;
				}

				//如果词不为空，则从当前DFA状态回溯，找到一个终态后停止，查询这个状态识别的词类型并打印
				while (this->endList.find(current) == this->endList.end()) {
					if (stack.size() == 0) {  //回退到栈空依然没找到终态，则报错
						cout << "throw LexicalAnalysisError" << endl;
						return false;
					}

					current = stack.top();
					stack.pop();
					iter--;
					word.erase(word.length() - 1);
				}

				cout << setiosflags(ios::left) << setw(20) << setfill(' ') << "词：" + word
					<< setiosflags(ios::left) << setw(20) << setfill(' ') << "匹配：" + this->endMap[current] << endl;
				
				//清空word和栈，状态回到初态
				word.clear();
				while (!stack.empty()) {
					stack.pop();
				}
				current = 0;
			}

			else {
				//非法字符，报错
				if (tokens.find(*iter) == string::npos) {
					cout << "throw LexIllegalTokenError" << endl;
					return false;
				}

				const int next = this->stateTransTable[current][tokens.find(*iter) + 1];  //查状态转移矩阵得到下一状态

				//不能接收该字符
				if (next == -2) {
					//若词为空
					if (stack.size() == 0) {
						cout << "throw LexicalAnalysisError" << endl;
						return false;
					}

					//如果词不为空，则从当前DFA状态回溯，找到一个终态后停止，查询这个状态识别的词类型并打印
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

					cout << setiosflags(ios::left) << setw(20) << setfill(' ') << "词：" + word
						<< setiosflags(ios::left) << setw(20) << setfill(' ') << "匹配：" + this->endMap[current] << endl;
					
					//清空word和栈，状态回到初态，同时当前字符没有进入新生成的词内，迭代器返回一个单位
					word.clear();
					while (!stack.empty()) {
						stack.pop();
					}
					current = 0;
					iter--;
				}

				//能接收该字符，接受该字符并进行状态迁移
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
	  函数名称：WriteLexicalAnalysis
	  功    能：模拟总控程序对源代码进行词法分析（输出到文件）
	  输入参数：string input：源代码输入
				ofstream& out：输出的文件流
				map<string, string>& regularDescriptionMap：正规式和语法分析终结符的映射表
	  返 回 值：bool：词法分析是否通过
	  说    明：应当根据总控程序方便读取的方式写入
	***************************************************************************/
	bool DFA::WriteLexicalAnalysis(string input, ofstream& out, map<string, string>& regularDescriptionMap) {
		stack<int> stack;
		string::iterator iter = input.begin();
		int current = 0;
		string word;

		while (iter != input.end()) {
			//获取一个字符的过程
			if (*iter == ' ' || *iter == '\n' || *iter == '\r') {
				if (stack.size() == 0) {  //多个连续的分隔符出现，则忽略
					iter++;
					continue;
				}

				//如果词不为空，则从当前DFA状态回溯，找到一个终态后停止，查询这个状态识别的词类型并打印
				while (this->endList.find(current) == this->endList.end()) {
					if (stack.size() == 0) {  //回退到栈空依然没找到终态，则报错
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
				//清空word和栈，状态回到初态
				word.clear();
				while (!stack.empty()) {
					stack.pop();
				}
				current = 0;
			}


			else {
				//非法字符，报错
				if (tokens.find(*iter) == string::npos) {
					out << "throw LexIllegalTokenError" << endl;
					return false;
				}

				const int next = this->stateTransTable[current][tokens.find(*iter) + 1];

				//不能接收该字符
				if (next == -2) {
					//若词为空
					if (stack.size() == 0) {
						out << "throw LexicalAnalysisError" << endl;
						return false;
					}

					//如果词不为空，则从当前DFA状态回溯，找到一个终态后停止，查询这个状态识别的词类型并打印
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
					//清空word和栈，状态回到初态，同时当前字符没有进入新生成的词内，迭代器返回一个单位
					word.clear();
					while (!stack.empty()) {
						stack.pop();
					}
					current = 0;
					iter--;
				}

				//能接收该字符，接受该字符并进行状态迁移
				else {
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