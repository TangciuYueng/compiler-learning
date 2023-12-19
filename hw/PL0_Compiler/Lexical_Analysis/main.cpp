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
  �������ƣ�DevideRegular
  ��    �ܣ���������ʽ�ֽ�ΪС����ʽ
  ���������string& regular��������ʽ
  �� �� ֵ��vector<string>���ֽ���С����ʽ
  ˵    ����ֻ�������Ļ�����ֽ⡣���������ʽΪ��PROGRAM|l(l|d)`���ֽ��ΪPROGRAM��l(l|d)`
***************************************************************************/
vector<string> DevideRegular(string& regular) {
	vector<string> ret;
	string div;
	string::iterator iter = regular.begin();
	int cut = 0;

	while (1) {
		if (iter == regular.end() && div != "") {  //����������С����ʽ��Ϊ�գ�Ҳ���뷵�ص�vector��
			ret.push_back(div);
			break;
		}

		if (*iter == '|' && cut == 0) {  //�������������㣬�򽫵�ǰС����ʽ���뷵�ص�vector�У��������
			ret.push_back(div);
			div.clear();
		}
		else {  //�������ŵ���cut��ֵ��Ϊ0˵�����������С����ѵ�ǰ�ַ�����С����ʽ
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
  �������ƣ�ResetBucket
  ��    �ܣ����滻�ķ����Ż���Բ����
  ���������string& str��ԭ����ʽ
  �� �� ֵ��vector<string>��ת���������ʽ
  ˵    ����������ʽ�У�������Ϊ����ʽ�������źʹʷ��ַ�������ȫ��ͬ���ʴʷ��ַ��滻Ϊ�����Ž�������
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
  �������ƣ�RegularDescriptionMap
  ��    �ܣ�����һ������ʽ���﷨�������ŵ�ӳ���
  �����������
  �� �� ֵ��map<string, string>��ӳ���
  ˵    ������Ȼһ������ʽ��Ӧ�﷨������һ���ս�����ڽ��ʽ����﷨����ǰ��Ҫ֪�������﷨�����ж�Ӧʲô�ս��
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
  �������ƣ�main
  ��    �ܣ��ʷ�����������
  �����������
  �� �� ֵ��0
  ˵    �����˴�ֱ�������ɵ�DFA�����˴ʷ�����������Ӧ��ֻ��DFAд���ļ����ܿس����ȡ
***************************************************************************/
int main() {
	//�ʷ�����������ʽ����`��ʾ�հ����÷������滻�ʷ��ַ���Բ���ţ�
	string regular = "PROGRAM|BEGIN|END|CONST|VAR|WHILE|DO|IF|THEN|l(l|d)`|dd`|+|-|*|/|:=|=|<>|>|>=|<|<=|[|]|;|,";
	
	//�ַ�����������������������ַ�����
	string tokens = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz()+-*/:=<>;,";
	
	//�õ�����ʽ���﷨�������ŵ�ӳ���
	map<string, string> regularDescriptionMap = RegularDescriptionMap();

	vector<string> regulars = DevideRegular(regular);  //��������ʽ
	const int methodCount = regulars.size();           //ȡ��С����ʽ����
	set<int> totalStartList, totalEndList;             //����ʼ̬�б�����̬�б�
	map<int, NFAEndInfo> totalEndInfoMap;              //����̬��Ϣ��
	NFA* nfas = new NFA[methodCount];                  //����СNFA

	//��ÿ��С����ʽ�ֱ𹹽�NFA
	for (int i = 0; i < methodCount; i++)
	{
		string origin = regulars[i];
		RegularCalculator::PostfixPrep(regulars[i]);  //Ԥ����
		RegularCalculator::Postfix(regulars[i]);      //ת�ɺ�׺���ʽ
		ResetBracket(regulars[i]);                    //�ѷ����Ż���Բ����
		RegularCalculator::Calculation(regulars[i] += '#', nfas[i]);  //���Ͻ�����������NFA

		NFAPartInfo nfaInfo = nfas[i].GetInfo();  //��ȡ��NFA��ʼ̬����̬��ţ��������б���
		totalStartList.insert(nfaInfo.start);     //����NFA��ʼ̬��ż�������ʼ̬�б�
		totalEndList.insert(nfaInfo.end);         //����NFA��̬��ż�������̬�б�
		
		ResetBracket(origin);  //�ѷ����Ż���Բ����
		totalEndInfoMap.emplace(nfaInfo.end, NFAEndInfo(origin, i));  //����NFA��̬����Լ���ʶ�������ʽ��������̬��Ϣ��
	}

	//�½�һ��״̬��������������һ���Ż�������СNFA����ʼ̬�����ɴ�NFA
	const int newStart = NFA::stateCount;
	NFA::stateCount++;
	for (int start : totalStartList) {
		NFA::totalTransList.push_back(NFATrans(newStart, start, "<epsilon>"));
	}

	//������ʼ̬�б�����̬�б�����̬��Ϣ������NFA
	NFA totalNFA;
	totalNFA.MakeTotalNFA(newStart, totalEndList, totalEndInfoMap);
	//totalNFA.Show();
	delete[] nfas;
	
	DFA dfa = DFA(
		tokens,
		totalNFA.GetTransList(),
		totalNFA.GetInfo().start,
		totalNFA.GetEndList(),
		totalNFA.GetEndInfoMap());  //��NFA��Ϣ��ʼ��DFA
	dfa.SubsetMethod();          //�Ӽ�����ȡ״̬ת�ƾ���
	dfa.ShowStateTransTable();   //��ӡ״̬ת�ƾ���
	//dfa.Simplify();

	ofstream out = ofstream("C:/Users/CHEMISTRYMASTER/Desktop/DFA.csv");  //��DFA״̬ת����Ϣд���ļ�
	dfa.WriteStateTransTable(out);
	out.close();

	/*
		��д����
		�ؼ��ֺ���ո�
		˫Ŀ��������ɷ�

		��������
		�ƥ��
		����ƥ��
		�ո񡢻��з�ǿ�Ƴɴ�
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
	
	//dfa.LexicalAnalysis(program, regularDescriptionMap);  //���дʷ�����
	
	//ofstream out("C:/Users/CHEMISTRYMASTER/Desktop/result.txt");
	//dfa.WriteLexicalAnalysis(program, out, regularDescriptionMap);
	
	return 0;
}