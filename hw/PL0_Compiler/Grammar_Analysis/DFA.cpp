#include"DFA.h"

namespace grammar {
	/***************************************************************************
	  函数名称：PreProductions
	  功    能：对文法中的产生式进行预处理
	  输入参数：const vector<Production>& grammar：文法（只读）
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void DFA::PreProductions(const vector<Production>& grammar) {
		int num = 0;

		for (Production production : grammar) {
			//1.给产生式编号
			this->productionToNum.emplace(production, num);
			this->numToProduction.emplace(num, production);

			//2.按产生式左部加入ref映射
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
	  函数名称：PreWord
	  功    能：对文法中的词进行预处理
	  输入参数：const set<string>& terminals：终结符集合（只读）
	            const set<string>& nonterminals：非终结符集合（只读）
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void DFA::PreWord(const set<string>& terminals, const set<string>& nonterminals) {
		for (string terminal : terminals) {
			this->FormFirst(Element(terminal, true));  //求FIRST()
		}

		for (string nonterminal : nonterminals) {
			this->FormFirst(Element(nonterminal, false));  //求FIRST()
		}
	}

	/***************************************************************************
	  函数名称：FormFirst
	  功    能：求出一个词的FIRST()
	  输入参数：const Element& element：要求FIRST()的词（只读）
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void DFA::FormFirst(const Element& element) {
		if (element.ifTerminal) {  //如果是终结符，则将一个只有它自己的集合加入
			set<string> newSet = { element.word };
			this->firstList.emplace(element.word, newSet);
		}

		else {  //否则递归求解非终结符的FIRST()
			this->FormFirstRecursive(element);
		}
	}

	/***************************************************************************
	  函数名称：FormFirstRecursive
	  功    能：求出一个词的FIRST()（递归部分）
	  输入参数：const Element& element：要求FIRST()的词（只读）
	  返 回 值：set<string>：该词的FIRST()
	  说    明：1.该算法无法处理非终结符都有左递归的文法
	            2.幸运的是我们在书写PL0的产生式时完全避免了左递归
				3.若要提升健壮性，也可参照自上而下分析中消除左递归的方法
	***************************************************************************/
	set<string> DFA::FormFirstRecursive(const Element& element) {
		if (element.ifTerminal) {  //终结符则返回只有它自己的集合
			return set<string>({ element.word });
		}

		auto place = this->firstList.find(element.word);  //非终结符如果已经求出也直接返回
		if (place != this->firstList.end()) {
			return place->second;
		}

		set<string> newSet;  //待求的FIRST()

		//求FIRST()需要查看所有以该词为左部的产生式
		for (int proNum : this->productionLeftRef.find(element.word)->second) {
			Production production = this->numToProduction.find(proNum)->second;  //根据序号查到产生式
			vector<Element>::iterator iter = production.right.begin();           //遍历产生式右边的元素

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

		this->firstList.emplace(element.word, newSet);  //能运行到此说明该元素的FIRST()已求出，进行记录
		return newSet;
	}

	/***************************************************************************
	  函数名称：GetFirst
	  功    能：查询一个词的FIRST()
	  输入参数：const Element& element：要求FIRST()的词（只读）
	  返 回 值：set<string>：该词的FIRST()
	  说    明：本函数建立在已求出所有词FIRST()的基础上
	***************************************************************************/
	set<string> DFA::GetFirst(const Element& word) {
		auto pair = this->firstList.find(word.word);

		if (pair == this->firstList.end()) {
			return set<string>();
		}

		return pair->second;
	}

	/***************************************************************************
	  函数名称：GetFirst
	  功    能：查询一个句子的FIRST()
	  输入参数：const vector<Element>& wordList：要求FIRST()的句子（只读）
	  返 回 值：set<string>：该词的FIRST()
	  说    明：本函数建立在已求出所有词FIRST()的基础上
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

	/***************************************************************************
	  函数名称：ShowFirstList
	  功    能：打印所有词的FIRST()
	  输入参数：无
	  返 回 值：无
	  说    明：本函数建立在已求出所有词FIRST()的基础上
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
	  函数名称：GetClosure
	  功    能：求一个给定LR(1)项目集的闭包
	  输入参数：DFAState& state：LR(1)项目集
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void DFA::GetClosure(DFAState& state) {
		for (size_t i = 0; i < state.productions.size(); i++) {  //遍历每个项目（即使是新加入的也能遍历到）
			//若该项目是归约项目，或该项目只能接收终结符，则跳过
			if (state.productions[i].end() || state.productions[i].GetPoint().ifTerminal)
				continue;

			set<string> newOutlook;                     //新项目的展望符集
			int next = state.productions[i].point + 1;  //取该产生式圆点的下一个位置

			//如果再下一个位置圆点到头了，则新项目的展望符集就是当前项目的
			if (state.productions[i].nextEnd()) {
				for (string outlook : state.productions[i].outlook) {
					newOutlook.insert(outlook);
				}
			}
			//如果还没到头
			else {
				for (string outlook : state.productions[i].outlook) {
					vector<Element> part;

					//取圆点后移一位后，产生式右部剩下的部分
					for (size_t j = state.productions[i].point + 1; j < state.productions[i].right.size(); j++)
					{
						part.push_back(state.productions[i].right[j]);
					}

					//同时加上原产生式的展望符
					part.push_back(Element(outlook, true));

					//求出该部分的FIRST()并到新项目的展望符集中
					set<string> partFirst = this->GetFirst(part);
					set_union(partFirst.begin(), partFirst.end(), newOutlook.begin(), newOutlook.end(),
						inserter(newOutlook, newOutlook.begin()));
				}
				
				newOutlook.erase("EPSILON");  //展望符无空字
			}

			//求出展望符集后，设该产生式能接受非终结符N
			//遍历所有N打头的产生式，化为LR(1)项目并加上求得的outlook，加入当前状态
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
	  函数名称：LR1
	  功    能：使用LR(1)分析法获取文法的LR(1)分析表
	  输入参数：无
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void DFA::LR1() {
		DFAState initState;            //初始状态
		queue<DFAState> openlist;      //扩展列表
		map<vector<DFAProduction>, int> existList;  //现有状态集

		//将文法起始符为左部的产生式加上只有结束符的展望符集作为一个LR(1)项目，加入初始状态，并对其求闭包
		Production production = this->numToProduction.find(0)->second;
		initState.num = 0;
		set<string> initOutLook = set<string>({ "TERMINAL" });
		initState.productions.push_back(DFAProduction(production.left, production.right, initOutLook));
		this->GetClosure(initState);

		existList.emplace(initState.productions, 0);          //将初始状态结点加入到现有状态列表中
		this->tableLR1.push_back(map<string, set<Trans>>());  //LR(1)分析表添加新的一行
		openlist.push(initState);  //将初始状态加入扩展列表

		while (!openlist.empty()) {
			DFAState current = openlist.front();
			openlist.pop();
			map<Element, DFAState> prepOpenlist;  //当前状态能转移到的所有状态集合，简称小openlist

			//遍历当前状态所有产生式
			for (DFAProduction dfaProduction : current.productions) {
				if (dfaProduction.end()) {  //若圆点已到头，则忽略
					continue;
				}

				Element recv = dfaProduction.GetPoint();  //取该LR(1)项目要接收的符号
				auto place = prepOpenlist.find(dfaProduction.GetPoint());
				dfaProduction.add();  //点后移

				//若当前状态接收该符号的状态已经在小openlist中，则直接将圆点移动后的LR(1)项目加入该状态
				if (place != prepOpenlist.end()) {
					place->second.productions.push_back(dfaProduction);
				}
				//若当前状态接收该符号的状态不在小openlist中，则小openlist新增一个状态，里面只有圆点移动后的当前LR(1)项目
				else {
					vector<DFAProduction> newProductions = { dfaProduction };
					DFAState newState(-1, newProductions);
					prepOpenlist.emplace(recv, newState);
				}
			}

			//遍历小openlist
			for (auto pair : prepOpenlist) {
				Element edge = pair.first;     //接收的符号
				DFAState state = pair.second;  //状态
				this->GetClosure(state);       //对状态求闭包
				int startPos = existList.find(current.productions)->second;  //当前状态一定已经存在
				auto end = existList.find(state.productions);

				//状态在当前DFA中已经存在
				if (end != existList.end()) {
					this->AddTableLR1Edge(startPos, end->second, edge);  //分析表添加迁移动作
					if (state.num == 0)
						this->AddTableLR1Vertex(state);  //初始状态已经存在，需要分析表添加归约动作
				}
				//状态在当前DFA中不存在
				else {
					state.num = existList.size();         //给状态编号
					cout << "新状态：" << state.num << endl;
					existList.emplace(state.productions, state.num);  //将新状态进行记录
					openlist.push(state);                 //将新状态加入扩展列表
					this->tableLR1.push_back(map<string, set<Trans>>());  //LR(1)分析表新增一行
					this->AddTableLR1Edge(startPos, state.num, edge);     //分析表添加迁移动作
					this->AddTableLR1Vertex(state);                       //分析表添加归约动作
				}
			}
		}
	}

	/***************************************************************************
	  函数名称：AddTableLR1Edge
	  功    能：向LR(1)分析表添加迁移动作
	  输入参数：const int start：起始状态（只读）
				const int end：目的状态（只读）
				const Element& word：接收词（只读）
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void DFA::AddTableLR1Edge(const int start, const int end, const Element& word) {
		auto transPlace = this->tableLR1[start].find(word.word);  //在LR(1)分析表的起始状态那一行找到接收word那一列对应的动作集合

		//若该动作集合存在，则加入该动作
		if (transPlace != this->tableLR1[start].end()) {
			transPlace->second.insert(Trans(word.ifTerminal ? TransType::ACTION_SHIFT : TransType::GOTO, end));  //根据接收词是否为终结符，决定动作是shift还是goto
		}
		//若该动作集合不存在，则创建该集合，只有当前一个动作
		else {
			set<Trans> newSet;
			newSet.insert(Trans(word.ifTerminal ? TransType::ACTION_SHIFT : TransType::GOTO, end));  //根据接收词是否为终结符，决定动作是shift还是goto
			this->tableLR1[start].emplace(word.word, newSet);
		}
	}

	/***************************************************************************
	  函数名称：AddTableLR1Vertex
	  功    能：向LR(1)分析表添加归约动作
	  输入参数：const DFAState& state：待检查状态（只读）
	  返 回 值：无
	  说    明：无
	***************************************************************************/
	void DFA::AddTableLR1Vertex(const DFAState& state) {
		for (DFAProduction dfaProduction : state.productions) {  //遍历该状态产生式
			if (dfaProduction.end()) {  //只有圆点到头，LR(1)项目才是规约项目
				Production production = Production(dfaProduction.left, dfaProduction.right);
				const int pNum = this->productionToNum.find(production)->second;

				//起始产生式，acc
				if (dfaProduction.left == this->startWord) {
					set<Trans> newSet;
					newSet.insert(Trans(TransType::ACTION_ACC, pNum));
					this->tableLR1[state.num].emplace("TERMINAL", newSet);
				}
				//其它产生式，reduce
				else {
					for (string outlook : dfaProduction.outlook) {  //在每个展望符处添加规约动作
						auto transPlace = this->tableLR1[state.num].find(outlook);

						//动作集合存在，直接添加
						if (transPlace != this->tableLR1[state.num].end()) {
							transPlace->second.insert(Trans(TransType::ACTION_REDUCE, pNum));
						}
						//动作集合不存在，创建动作集合
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
	  函数名称：FormatTrans
	  功    能：将动作集合格式化成字符串，便于输出
	  输入参数：const set<Trans>& set：待格式化的动作集合（只读）
	  返 回 值：string：格式化后的字符串
	  说    明：无
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
	  函数名称：ShowTableLR1
	  功    能：格式化打印LR(1)分析表
	  输入参数：const set<string>& terminals：终结符集合（只读）
				const set<string>& nonterminals：非终结符集合（只读）
	  返 回 值：无
	  说    明：本函数同时统计了冲突状态，且为调试用
	***************************************************************************/
	void DFA::ShowTableLR1(const set<string>& terminals, const set<string>& nonterminals) {
		set<int> dupStates;  //冲突状态集合
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
					if (place->second.size() != 1)  //若动作集合大小大于1，该状态为冲突状态，进行记录
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
					if (place->second.size() != 1)  //若动作集合大小大于1，该状态为冲突状态，进行记录
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
	  函数名称：WriteTableLR1
	  功    能：将LR(1)分析表输出到csv表格中
	  输入参数：ofstream& out：csv文件
				const set<string>& terminals：终结符集合（只读）
				const set<string>& nonterminals：非终结符集合（只读）
	  返 回 值：无
	  说    明：本函数同时写入了冲突状态，且如何写入应当看总控程序如何方便读取
	***************************************************************************/
	void DFA::WriteTableLR1(ofstream& out, const set<string>& terminals, const set<string>& nonterminals) {
		set<int> dupStates;  //冲突状态集合
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
					if (place->second.size() != 1)  //若动作集合大小大于1，该状态为冲突状态，进行记录
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
					if (place->second.size() != 1)  //若动作集合大小大于1，该状态为冲突状态，进行记录
						dupStates.insert(i);

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

	/***************************************************************************
	  函数名称：ReadWords
	  功    能：将一份词法分析结果读入
	  输入参数：const char* filename：文件名（只读）
	  返 回 值：vector<string>：词语列表
	  说    明：不应出现，应当移植到总控程序中
	***************************************************************************/
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

	/***************************************************************************
	  函数名称：GrammarAnalysis
	  功    能：模拟总控程序对源代码进行语法分析（输出到控制台）
	  输入参数：vector<string>& input：源代码词法分析结果输入
	  返 回 值：bool：语法分析是否通过
	  说    明：不应出现，应当移植到总控程序中
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