#include<iostream>
#include<set>
#include<vector>
#include"Production.h"
#include"DFAProduction.h"
#include"DFA.h"
using namespace std;
using namespace grammar;

int main() {
	//终结符表
	const set<string> terminals = {
		"PROGRAM",
		"BEGIN",
		"END",
		"CONST",
		"VAR",
		"WHILE",
		"DO",
		"IF",
		"THEN",
		"ID",
		"NUM",
		"PLUS",
		"MINUS",
		"TIMES",
		"DIV",
		"ASSIGN",
		"EQUAL",
		"NEQ",
		"LT",
		"GT",
		"LE",
		"GE",
		"LPAREN",
		"RPAREN",
		"SEMI",
		"COMMA",
		"EPSILON",
		"TERMINAL"
	};

	//非终结符表
	const set<string> nonterminals = {
		"Program`",
		"Program",
		"SubProgram",
		"ConstantDeclaration",
		"ConstantDefinition",
		"VariableDeclaration",
		"Statement",
		"NextConstantDefinition",
		"NextID",
		"CompoundStatement",
		"NextStatement",
		"AssignmentStatement",
		"ConditionStatement",
		"LoopStatement",
		"EmptyStatement",
		"Identifier",
		"Expression",
		"Term",
		"Factor",
		"Condition",
		"M",
		"RelationalOperator"
	};

	//文法
	const vector<Production> grammar = {
		Production(
			"Program`",
			{ Element("Program", false) }
		),  //LR分析时添加的产生式
		Production(
			"Program",
			{ Element("PROGRAM", true), Element("ID", true), Element("SubProgram", false) }
		),
		Production(
			"SubProgram",
			{ Element("ConstantDeclaration", false), Element("VariableDeclaration", false), Element("Statement", false) }
		),
		Production(
			"SubProgram",
			{ Element("ConstantDeclaration", false), Element("Statement", false) }
		),
		Production(
			"SubProgram",
			{ Element("VariableDeclaration", false), Element("Statement", false) }
		),
		Production(
			"SubProgram",
			{ Element("Statement", false) }
		),
		Production(
			"ConstantDeclaration",
			{ Element("CONST", true), Element("ConstantDefinition", false), Element("NextConstantDefinition", false) }
		),
		Production(
			"NextConstantDefinition",
			{ Element("COMMA", true), Element("ConstantDefinition", false), Element("NextConstantDefinition", false) }
		),
		Production(
			"NextConstantDefinition",
			{ Element("SEMI", true) }
		),
		Production(
			"ConstantDefinition",
			{ Element("ID", true), Element("ASSIGN", true), Element("NUM", true) }
		),
		Production(
			"VariableDeclaration",
			{ Element("VAR", true), Element("ID", true), Element("NextID", false) }
		),
		Production(
			"NextID",
			{ Element("COMMA", true), Element("ID", true), Element("NextID", false) }
		),
		Production(
			"NextID",
			{ Element("SEMI", true) }
		),
		Production(
			"CompoundStatement",
			{ Element("BEGIN", true), Element("Statement", false), Element("NextStatement", false) }
		),
		Production(
			"NextStatement",
			{ Element("SEMI", true), Element("M", false), Element("Statement", false), Element("NextStatement", false)}
		),
		Production(
			"NextStatement",
			{ Element("END", true) }
		),
		Production(
			"Statement",
			{ Element("AssignmentStatement", false) }
		),
		Production(
			"Statement",
			{ Element("ConditionStatement", false) }
		),
		Production(
			"Statement",
			{ Element("LoopStatement", false) }
		),
		Production(
			"Statement",
			{ Element("CompoundStatement", false) }
		),
		Production(
			"Statement",
			{ Element("EmptyStatement", false) }
		),
		Production(
			"AssignmentStatement",
			{ Element("Identifier", false), Element("ASSIGN", true), Element("Expression", false) }
		),
		Production(
			"Expression",
			{ Element("PLUS", true), Element("Term", false) }
		),
		Production(
			"Expression",
			{ Element("MINUS", true), Element("Term", false) }
		),
		Production(
			"Expression",
			{ Element("Term", false) }
		),
		Production(
			"Expression",
			{ Element("Expression", false), Element("PLUS", true), Element("Term", false) }
		),
		Production(
			"Expression",
			{ Element("Expression", false), Element("MINUS", true), Element("Term", false) }
		),
		Production("Term",
			{ Element("Factor", false) }
		),
		Production("Term",
			{ Element("Term", false), Element("TIMES", true), Element("Factor", false) }
		),
		Production("Term",
			{ Element("Term", false), Element("DIV", true), Element("Factor", false) }
		),
		Production("Factor",
			{ Element("Identifier", false) }
		),
		Production("Factor",
			{ Element("NUM", true) }
		),
		Production("Factor",
			{ Element("LPAREN", true), Element("Expression", false), Element("RPAREN", true) }
		),
		Production("ConditionStatement",
			{ Element("IF", true), Element("Condition", false), Element("THEN", true), Element("M", false), Element("Statement", false) }
		),
		Production("M",
			{ Element("EPSILON", true) }
		),
		Production("LoopStatement",
			{ Element("WHILE", true), Element("M", false), Element("Condition", false), Element("DO", true), Element("M", false), Element("Statement", false) }
		),
		Production("Condition",
			{ Element("Expression", false), Element("RelationalOperator", false), Element("Expression", false) }
		),
		Production("RelationalOperator",
			{ Element("EQUAL", true) }
		),
		Production("RelationalOperator",
			{ Element("NEQ", true) }
		),
		Production("RelationalOperator",
			{ Element("LT", true) }
		),
		Production("RelationalOperator",
			{ Element("LE", true) }
		),
		Production("RelationalOperator",
			{ Element("GT", true) }
		),
		Production("RelationalOperator",
			{ Element("GE", true) }
		),
		Production("EmptyStatement",
			{ Element("EPSILON", true) }
		),
		Production("Identifier",
			{ Element("ID", true)}
		)
	};

	//构造DFA
	DFA dfa = DFA(grammar, terminals, nonterminals, grammar[0].left);
	
	//LR(1)分析
	dfa.LR1();

	//打印所有符号的FIRST()
	//dfa.ShowFirstList();

	//打印LR(1)分析表
	//dfa.ShowTableLR1(terminals, nonterminals);

	//将LR(1)分析表写入表格文件
	//ofstream out("C:/Users/CHEMISTRYMASTER/Desktop/LR1.csv");
	//dfa.WriteTableLR1(out, terminals, nonterminals);
	//out.close();

	//读取词法分析结果，进行语法分析
	vector<string> words = dfa.ReadWords("C:/Users/CHEMISTRYMASTER/Desktop/result.txt");
	cout << dfa.GrammarAnalysis(words) << endl;
	return 0;
}