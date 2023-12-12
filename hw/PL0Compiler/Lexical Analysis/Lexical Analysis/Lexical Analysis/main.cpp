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
		totalStartList.insert(nfaInfo.start);
		totalEndList.insert(nfaInfo.end);

		
		ResetBracket(origin);  //�ѷ����Ż���Բ����
		totalEndInfoMap.emplace(nfaInfo.end, NFAEndInfo(origin, i));  //����NFA��̬����Լ���ʶ�������ʽ��������̬��Ϣ��
	}

	//�½�һ��״̬��������������һ���Ż�������СNFA����ʼ̬�����ɴ�NFA
	const int newStart = NFA::stateCount;
	NFA::stateCount++;
	for (int start : totalStartList) {
		NFA::totalTransList.push_back(NFATrans(newStart, start, "<epsilon>"));
	}

	//������NFA
	NFA totalNFA;
	totalNFA.MakeTotalNFA(newStart, totalEndList);
	totalNFA.SetEndInfoMap(totalEndInfoMap);
	//totalNFA.Show();
	
	DFA dfa = DFA(
		tokens,
		totalNFA.GetTransList(),
		totalNFA.GetInfo().start,
		totalNFA.GetEndList(),
		totalNFA.GetEndInfoMap());  //��NFA��Ϣ��ʼ��DFA
	dfa.SubsetMethod();          //�Ӽ�����ȡ״̬ת�ƾ���
	//dfa.ShowStateTransTable();   //��ӡ״̬ת�ƾ���
	ofstream out = ofstream("C:/Users/CHEMISTRYMASTER/Desktop/DFA.txt");  //��DFA״̬ת����Ϣд���ļ�
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
	dfa.LexicalAnalysis(program);  //���дʷ�����
	
	delete[] nfas;
}