class Element:
    def __init__(self, word, if_terminal):
        self.word = word
        self.if_terminal = if_terminal

    def __str__(self):
        return f'word({self.word})'

    def __repr__(self):
        return self.__str__()

    def __hash__(self):
        return hash(self.word)

    def __eq__(self, other):
        return self.word == other.word and self.if_terminal == other.if_terminal


class Production:
    def __init__(self, left, right):
        self.left = left
        if not right:
            self.right = [Element("EPSILON", True)]
        else:
            self.right = right

    def __str__(self):
        return f"{self.left} -> {self.right}"

    def __repr__(self):
        return self.__str__()

    def __hash__(self):
        return hash((self.left, tuple(hash(element) for element in self.right)))

    def __eq__(self, other):
        return self.left == other.left and self.right == other.right


class DFAProduction:
    def __init__(self, left: str, right, outlook, point=0):
        self.left = left
        self.right = [element for element in right if element.word != "EPSILON"]
        self.point = point
        self.outlook = outlook
    
    def __hash__(self):
        return hash((self.left, tuple((element.word, element.if_terminal) for element in self.right),
                     self.point, tuple(sorted(self.outlook))))

    def __eq__(self, other):
        if not isinstance(other, DFAProduction):
            return False
        return (
            self.left == other.left
            and len(self.right) == len(other.right)
            and all(x == y for x, y in zip(self.right, other.right))
            and self.point == other.point
            and sorted(self.outlook) == sorted(other.outlook)
        )

    def __str__(self):
        result = f"{self.left} -> "
        for i, element in enumerate(self.right):
            if i == self.point:
                result += "·"
            result += f"{element.word} "
        if self.point == len(self.right):
            result += "·"
        result += "(" + ", ".join(self.outlook) + ")"
        return result

    def __repr__(self):
        return self.__str__()

    def begin(self) -> bool:
        return self.point == 0

    def end(self) -> bool:
        return self.point == len(self.right)

    def next_end(self) -> bool:
        return self.end() or self.point + 1 == len(self.right)

    def add(self) -> None:
        if not self.end():
            self.point += 1

    def get_point(self) -> Element:
        if not self.end():
            return self.right[self.point]
        return Element("ERROR", True)


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
    Production("CompoundStatement", [Element("BEGIN", True), Element("LastStatement", False), Element("END", True)]),
    Production("LastStatement", [
        Element("LastStatement", False),
        Element("SEMI", True),
        Element("M", False),
        Element("Statement", False),
    ]),
    Production("LastStatement", [Element("Statement", False)]),
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