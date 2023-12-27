terminals = {
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
}

nonterminals = {
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
}

from Production import Element, Production

grammar = [
    Production("Program`", [Element("Program", False)]),
    Production("Program", [Element("PROGRAM", True), Element("ID", True), Element("SubProgram", False)]),
    Production("SubProgram", [Element("ConstantDeclaration", False), Element("VariableDeclaration", False), Element("Statement", False)]),
    Production("SubProgram", [Element("ConstantDeclaration", False), Element("Statement", False)]),
    Production("SubProgram", [Element("VariableDeclaration", False), Element("Statement", False)]),
    Production("SubProgram", [Element("Statement", False)]),
    Production("ConstantDeclaration", [Element("CONST", True),  Element("ConstantDefinition", False), Element("NextConstantDefinition", False)]),
    Production("NextConstantDefinition", [Element("COMMA", True), Element("ConstantDefinition", False), Element("NextConstantDefinition", False)]),
    Production("NextConstantDefinition", [Element("SEMI", True)]),
    Production("ConstantDefinition", [Element("ID", True), Element("ASSIGN", True), Element("NUM", True)]),
    Production("VariableDeclaration", [Element("VAR", True), Element("ID", True), Element("NextID", False)]),
    Production("NextID", [Element("COMMA", True), Element("ID", True), Element("NextID", False)]),
    Production("NextID", [Element("SEMI", True)]),
    Production("CompoundStatement", [Element("BEGIN", True), Element("Statement", False), Element("NextStatement", False)]),
    Production("NextStatement", [
        Element("SEMI", True),
        Element("M", False),
        Element("Statement", False),
        Element("NextStatement", False)
    ]),
    Production("NextStatement", [Element("END", True)]),
    Production("Statement", [Element("AssignmentStatement", False)]),
    Production("Statement", [Element("ConditionStatement", False)]),
    Production("Statement", [Element("LoopStatement", False)]),
    Production("Statement", [Element("CompoundStatement", False)]),
    Production("Statement", [Element("EmptyStatement", False)]),
    Production("AssignmentStatement", [
        Element("Identifier", False),
        Element("ASSIGN", True),
        Element("Expression", False)
    ]),
    Production("Expression", [Element("PLUS", True), Element("Term", False)]),
    Production("Expression", [Element("MINUS", True), Element("Term", False)]),
    Production("Expression", [Element("Term", False)]),
    Production("Expression", [
        Element("Expression", False),
        Element("PLUS", True),
        Element("Term", False)
    ]),
    Production("Expression", [
        Element("Expression", False),
        Element("MINUS", True),
        Element("Term", False)
    ]),
    Production("Term", [Element("Factor", False)]),
    Production("Term", [
        Element("Term", False),
        Element("TIMES", True),
        Element("Factor", False)
    ]),
    Production("Term", [
        Element("Term", False),
        Element("DIV", True),
        Element("Factor", False)
    ]),
    Production("Factor", [Element("Identifier", False)]),
    Production("Factor", [Element("NUM", True)]),
    Production("Factor", [
        Element("LPAREN", True),
        Element("Expression", False),
        Element("RPAREN", True)
    ]),
    Production("ConditionStatement", [
        Element("IF", True),
        Element("Condition", False),
        Element("THEN", True),
        Element("M", False),
        Element("Statement", False)
    ]),
    Production("M", [Element("EPSILON", True)]),
    Production("LoopStatement", [
        Element("WHILE", True),
        Element("M", False),
        Element("Condition", False),
        Element("DO", True),
        Element("M", False),
        Element("Statement", False)
    ]),
    Production("Condition", [
        Element("Expression", False),
        Element("RelationalOperator", False),
        Element("Expression", False)
    ]),
    Production("RelationalOperator", [Element("EQUAL", True)]),
    Production("RelationalOperator", [Element("NEQ", True)]),
    Production("RelationalOperator", [Element("LT", True)]),
    Production("RelationalOperator", [Element("LE", True)]),
    Production("RelationalOperator", [Element("GT", True)]),
    Production("RelationalOperator", [Element("GE", True)]),
    Production("EmptyStatement", [Element("EPSILON", True)]),
    Production("Identifier", [Element("ID", True)])
]

from DFA import DFA

dfa = DFA(grammar, terminals, nonterminals, grammar[0].left)

dfa.lr1()

dfa.show_table_lr1(terminals, nonterminals)
