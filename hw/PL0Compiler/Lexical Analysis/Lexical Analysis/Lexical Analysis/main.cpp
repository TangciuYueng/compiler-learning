#include<iostream>
#include<stack>
#include<fstream>
#include"RegularCalculator.h"
#include"NFA.h"
#include"DFA.h"
using namespace std;
using namespace automat;

/* | < $ < ` */
vector<string> DevideRegular(string& regular) {
	vector<string> ret;
	string div;
	string::iterator iter = regular.begin();
	int cut = 0;

	while (1) {
		if (iter == regular.end() && div != "") {
			ret.push_back(div);
			break;
		}

		if (*iter == '|' && cut == 0) {
			ret.push_back(div);
			div.clear();
		}
		else {
			if (*iter == '(')
				cut++;
			else if (*iter == ')')
				cut--;

			div += *iter;
		}

		iter++;
	}

	return ret;
}

void ResetBracket(string& str) {
	string::iterator iter = str.begin();
	
	while (iter != str.end()) {
		if (*iter == '[')
			*iter = '(';
		else if (*iter == ']')
			*iter = ')';

		iter++;
	}
}

int main() {
	string regular = "PROGRAM|BEGIN|END|CONST|VAR|WHILE|DO|IF|THEN|l(l|d)`|dd`|+|-|*|/|:=|=|<>|>|>=|<|<=|[|]|;|,";
	string tokens = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz()+-*/:=<>;,";
	
	vector<string> regulars = DevideRegular(regular);  //划分正规式
	const int methodCount = regulars.size();           //取得小正规式个数
	set<int> totalStartList, totalEndList;             //总起始态列表、总终态列表
	map<int, NFAEndInfo> totalEndInfoMap;              //总终态信息表
	NFA* nfas = new NFA[methodCount];                  //各个小NFA

	//给每个小正规式分别构建NFA
	for (int i = 0; i < methodCount; i++)
	{
		string origin = regulars[i];
		RegularCalculator::PostfixPrep(regulars[i]);  //预处理
		RegularCalculator::Postfix(regulars[i]);      //转成后缀表达式
		ResetBracket(regulars[i]);                    //把方括号换回圆括号
		RegularCalculator::Calculation(regulars[i] += '#', nfas[i]);  //加上结束符，构建NFA

		NFAPartInfo nfaInfo = nfas[i].GetInfo();  //获取该NFA起始态、终态编号，加入总列表中
		totalStartList.insert(nfaInfo.start);
		totalEndList.insert(nfaInfo.end);

		
		ResetBracket(origin);  //把方括号换回圆括号
		totalEndInfoMap.emplace(nfaInfo.end, NFAEndInfo(origin, i));  //将该NFA终态编号以及能识别的正规式加入总终态信息表
	}

	//新建一个状态，从它出发经过一条ε弧到所有小NFA的起始态，构成大NFA
	const int newStart = NFA::stateCount;
	NFA::stateCount++;
	for (int start : totalStartList) {
		NFA::totalTransList.push_back(NFATrans(newStart, start, "<epsilon>"));
	}

	//构建大NFA
	NFA totalNFA;
	totalNFA.MakeTotalNFA(newStart, totalEndList);
	totalNFA.SetEndInfoMap(totalEndInfoMap);
	//totalNFA.Show();
	
	DFA dfa = DFA(
		tokens,
		totalNFA.GetTransList(),
		totalNFA.GetInfo().start,
		totalNFA.GetEndList(),
		totalNFA.GetEndInfoMap());  //用NFA信息初始化DFA
	dfa.SubsetMethod();          //子集法获取状态转移矩阵
	//dfa.ShowStateTransTable();   //打印状态转移矩阵
	ofstream out = ofstream("C:/Users/CHEMISTRYMASTER/Desktop/DFA.txt");  //将DFA状态转移信息写入文件
	dfa.WriteStateTransTable(out);
	out.close();

	/*
		书写规则：
		关键字后跟空格
		双目运算符不可分

		分析规则：
		最长匹配
		优先匹配
		空格、换行符强制成词
	*/
	/*string program = string() +
		"PROGRAM add\n"
		"VAR x, y;\n"
		"BEGIN\n"
			"x := 1;\n"
			"y := 2;\n"
			"WHILE x<5 DO x := x+1;\n"
			"IF y>0 THEN y := y-1;\n"
			"y := y + x;\n"
		"END\n";*/
	string program = string() +
		"PROGRAM add\n"
		"VAR x1, y, helloworld;\n"
		"CONST x2 := 100, y1 := 1000;\n"
		"BEGIN\n"
			"x1 := 1;\n"
			"y := 2;\n"
			"helloworld := 65536;\n"
			"WHILE x1<5 DO x := x1+helloworld;\n"
			"IF y <> 0 THEN y := y-1;\n"
			"IF x1 <= 200 THEN x := 100;\n"
			"IF x1 >= 200 THEN y := x + y - (x2*y1/helloworld);\n"
			"y := y + x1;\n"
		"END\n";
	dfa.LexicalAnalysis(program);  //进行词法分析
	
	delete[] nfas;
}