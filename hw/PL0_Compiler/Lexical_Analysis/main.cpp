#include<iostream>
#include<stack>
#include<fstream>
#include<Windows.h>
#include"RegularCalculator.h"
#include"NFA.h"
#include"DFA.h"
using namespace std;
using namespace automat;

/***************************************************************************
  函数名称：DevideRegular
  功    能：将大正规式分解为小正规式
  输入参数：string& regular：大正规式
  返 回 值：vector<string>：分解后的小正规式
  说    明：只按最外层的或运算分解。例如大正规式为：PROGRAM|l(l|d)`，分解后为PROGRAM和l(l|d)`
***************************************************************************/
vector<string> DevideRegular(string& regular) {
	vector<string> ret;
	string div;
	string::iterator iter = regular.begin();
	int cut = 0;

	while (1) {
		if (iter == regular.end() && div != "") {  //遍历完后如果小正规式不为空，也加入返回的vector中
			ret.push_back(div);
			break;
		}

		if (*iter == '|' && cut == 0) {  //遇到最外层或运算，则将当前小正规式加入返回的vector中，变量清空
			ret.push_back(div);
			div.clear();
		}
		else {  //遇到括号调整cut的值，为0说明不在括号中。并把当前字符加入小正规式
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

/***************************************************************************
  函数名称：ResetBucket
  功    能：将替换的方括号换回圆括号
  输入参数：string& str：原正规式
  返 回 值：vector<string>：转换后的正规式
  说    明：大正规式中，括号作为正规式运算括号和词法字符意义完全不同，故词法字符替换为方括号进行区分
***************************************************************************/
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

/***************************************************************************
  函数名称：RegularDescriptionMap
  功    能：返回一个正规式和语法分析符号的映射表
  输入参数：无
  返 回 值：map<string, string>：映射表
  说    明：显然一种正规式对应语法分析的一种终结符，在将词交给语法分析前，要知道它在语法分析中对应什么终结符
***************************************************************************/
map<string, string> RegularDescriptionMap() {
	map<string, string> ret;

	ret.emplace("PROGRAM", "PROGRAM");
	ret.emplace("BEGIN", "BEGIN");
	ret.emplace("END", "END");
	ret.emplace("CONST", "CONST");
	ret.emplace("VAR", "VAR");
	ret.emplace("WHILE", "WHILE");
	ret.emplace("DO", "DO");
	ret.emplace("IF", "IF");
	ret.emplace("THEN", "THEN");
	ret.emplace("l(l|d)`", "ID");
	ret.emplace("dd`", "NUM");
	ret.emplace("+", "PLUS");
	ret.emplace("-", "MINUS");
	ret.emplace("*", "TIMES");
	ret.emplace("/", "DIV");
	ret.emplace(":=", "ASSIGN");
	ret.emplace("=", "EQUAL");
	ret.emplace("<>", "NEQ");
	ret.emplace(">", "GT");
	ret.emplace(">=", "GE");
	ret.emplace("<", "LT");
	ret.emplace("<=", "LE");
	ret.emplace("(", "LPAREN");
	ret.emplace(")", "RPAREN");
	ret.emplace(";", "SEMI");
	ret.emplace(",", "COMMA");

	return ret;
}

/***************************************************************************
  函数名称：main
  功    能：词法分析主程序
  输入参数：无
  返 回 值：0
  说    明：此处直接用生成的DFA进行了词法分析，后续应当只将DFA写进文件供总控程序读取
***************************************************************************/
int main() {
	//词法规则总正规式（用`表示闭包，用方括号替换词法字符的圆括号）
	string regular = "PROGRAM|BEGIN|END|CONST|VAR|WHILE|DO|IF|THEN|l(l|d)`|dd`|+|-|*|/|:=|=|<>|>|>=|<|<=|[|]|;|,";
	
	//字符表，即程序中所有有意义的字符集合
	string tokens = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz()+-*/:=<>;,";
	
	//得到正规式和语法分析符号的映射表
	map<string, string> regularDescriptionMap = RegularDescriptionMap();

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
		totalStartList.insert(nfaInfo.start);     //将该NFA起始态编号加入总起始态列表
		totalEndList.insert(nfaInfo.end);         //将该NFA终态编号加入总终态列表
		
		ResetBracket(origin);  //把方括号换回圆括号
		totalEndInfoMap.emplace(nfaInfo.end, NFAEndInfo(origin, i));  //将该NFA终态编号以及能识别的正规式加入总终态信息表
	}

	//新建一个状态，从它出发经过一条ε弧到所有小NFA的起始态，构成大NFA
	const int newStart = NFA::stateCount;
	NFA::stateCount++;
	for (int start : totalStartList) {
		NFA::totalTransList.push_back(NFATrans(newStart, start, "<epsilon>"));
	}

	//用总起始态列表、总终态列表、总终态信息表构建大NFA
	NFA totalNFA;
	totalNFA.MakeTotalNFA(newStart, totalEndList, totalEndInfoMap);
	//totalNFA.Show();
	delete[] nfas;
	
	DFA dfa = DFA(
		tokens,
		totalNFA.GetTransList(),
		totalNFA.GetInfo().start,
		totalNFA.GetEndList(),
		totalNFA.GetEndInfoMap());  //用NFA信息初始化DFA
	dfa.SubsetMethod();          //子集法获取状态转移矩阵
	dfa.ShowStateTransTable();   //打印状态转移矩阵
	//dfa.Simplify();

	ofstream out = ofstream("C:/Users/CHEMISTRYMASTER/Desktop/DFA.csv");  //将DFA状态转移信息写入文件
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
	string program = string() +
		"PROGRAM add\n"
		"VAR x, y;\n"
		"BEGIN\n"
			"x := 1;\n"
			"y := 2;\n"
			"WHILE x<5 DO x := x+1;\n"
			"IF y>0 THEN y := y-1;\n"
			"y := y + x;\n"
		"END\n";
	/*string program = string() +
		"PROGRAM add\n"
		"CONST sb := 1000;\n"
		"VAR x1, y, helloworld;\n"
		"BEGIN\n"
			"x1 := 1;\n"
			"y := 2;\n"
			"helloworld := 65536;\n"
			"WHILE x1<5 DO x := x1+helloworld;\n"
			"IF y <> 0 THEN y := y-1;\n"
			"IF x1 <= 200 THEN x := 100;\n"
			"IF x1 >= 200 THEN y := x + y - (x2*y1/helloworld);\n"
			"y := y + x1;\n"
		"END\n"
		"\n";*/
	
	//dfa.LexicalAnalysis(program, regularDescriptionMap);  //进行词法分析
	
	//ofstream out("C:/Users/CHEMISTRYMASTER/Desktop/result.txt");
	//dfa.WriteLexicalAnalysis(program, out, regularDescriptionMap);
	
	return 0;
}