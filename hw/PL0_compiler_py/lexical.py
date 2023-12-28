from enum import Enum, auto

class TokenType(Enum):
    # 关键字
    PROGRAM = auto()
    BEGIN = auto()
    END = auto()
    CONST = auto()
    VAR = auto()
    WHILE = auto()
    DO = auto()
    IF = auto()
    THEN = auto()

    # 标识符
    ID = auto()

    # 整数
    NUM = auto()

    # 算符和界符
    PLUS = auto()
    MINUS = auto()
    TIMES = auto()
    DIV = auto()
    ASSIGN = auto()
    EQUAL = auto()
    NEQ = auto()
    LT = auto()
    GT = auto()
    LE = auto()
    GE = auto()
    LPAREN = auto()
    RPAREN = auto()
    SEMI = auto()
    COMMA = auto()

single_symbol = {
    '+': TokenType.PLUS, 
    '-': TokenType.MINUS, 
    '*': TokenType.TIMES, 
    '/': TokenType.DIV, 
    '=': TokenType.EQUAL, 
    '(': TokenType.LPAREN, 
    ')': TokenType.RPAREN, 
    ';': TokenType.SEMI, 
    ',': TokenType.COMMA
}

def error(message):
    raise ValueError(f"Lexical error: {message}")

def next():
    global index
    while index < len(src):
        if src[index] in ['\n', ' ', '\t', '\r']:
            index += 1
            
        elif src[index].islower():
            word = ''
            while index < len(src) and (src[index].islower() or src[index].isdigit()):
                word += src[index]
                index += 1
            
            return (word, TokenType.ID)
        
        elif src[index].isdigit():
            num = 0
            while index < len(src) and src[index].isdigit():
                num = num * 10 + int(src[index])
                index += 1
            
            return (num, TokenType.NUM)

        elif src[index] == ':':
            index += 1
            if index < len(src) and src[index] == '=':
                index += 1
                return (":=", TokenType.ASSIGN)
            else:
                error("Expected '=' after ':'")

        elif src[index] == '<':
            index += 1
            if index < len(src) and src[index] == '=':
                index += 1
                return ('<=', TokenType.LE)
            elif index < len(src) and src[index] == '>':
                index += 1
                return ('<>', TokenType.NEQ)
            else:
                return ('<', TokenType.LT)
        
        elif src[index] == '>':
            index += 1
            if index < len(src) and src[index] == '=':
                index += 1
                return ('>=', TokenType.GE)
            else:
                return ('>', TokenType.GT)
        
        elif src[index] in single_symbol.keys():
            index += 1
            return (src[index - 1], single_symbol[src[index - 1]])

        elif src[index].isupper():
            keyword_found = False
            for keyword in ["PROGRAM", "BEGIN", "END", "CONST", "VAR", "WHILE", "DO", "IF", "THEN"]:
                if src[index:].startswith(keyword):
                    index += len(keyword)
                    return (keyword, TokenType[keyword])
                    keyword_found = True

            if not keyword_found:
                error("Invalid keyword")

        else:
            error("Invalid character")

src = (
        "PROGRAM add\n"
        "CONST sb := 1000;\n"
        "VAR x1, y, helloworld;\n"
        "BEGIN\n"
        "   x1 := 1;\n"
        "   y := 2;\n"
        "   helloworld := 65536;\n"
        "   WHILE x1<5 DO x := x1+helloworld;\n"
        "   IF y <> 0 THEN y := y-1;\n"
        "   IF x1 <= 200 THEN x := 100;\n"
        "   IF x1 >= 200 THEN y := x + y - (x2*y1/helloworld);\n"
        "   y := y + x1;\n"
        "END\n"
    )

# list<map<str, set<str>>>
lr1_table = []
symbols = []
import csv

def read_lr1_table_from_csv():
    global symbols, lr1_table
    with open('./LR1.csv', newline='') as file:
        csv_reader = csv.reader(file)

        first = True
        for row in csv_reader:
            if first:
                symbols = row[1:-1]
                first = False
            else:
                row = row[1:]
                lr1_table.append({})
                for i, action in enumerate(row):
                    if action != '':
                        lr1_table[-1][symbols[i]] = action
    lr1_table.pop()
    print(f'len {len(symbols)}')
    print(f'len {len(lr1_table)}')

import pickle
def save_lr1_table_to_pkl(file_path):
    global symbols, lr1_table
    with open(file_path, 'wb') as pkl_file:
        pickle.dump(symbols, pkl_file)
        pickle.dump(lr1_table, pkl_file)

def test():
    while index < len(src):
        try:
            lex = next()
            if lex:
                print(lex)
        except ValueError as e:
            print(e)
            break

def csv_to_pkl(file_path):
    read_lr1_table_from_csv()
    save_lr1_table_to_pkl(file_path)
    print('saved successfully')

def read_from_pkl(file_path):
    global symbols, lr1_table
    with open(file_path, 'rb') as pkl_file:
        symbols = pickle.load(pkl_file)
        lr1_table = pickle.load(pkl_file)

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

import re
def reduce_production(grammar, state_record, word_record, num):
    production = grammar[num]
    left = production.left
    reduce_length = len(production.right)
    if reduce_length == 1 and production.right[0].word == 'EPSILON':
        reduce_length = 0
    for _ in range(reduce_length):
        state_record.pop()
        word_record.pop()
    new_top = state_record[-1]
    new_action = lr1_table[new_top].get(left)
    new_match = re.search(r'\d+', new_action)
    new_num = int(new_match.group())
    state_record.append(new_num)
    word_record.append(left)
    print(f'reduce with {production} to state {state_record[-1]}')

def test_grammer_words(words):
    global symbols, lr1_table
    read_from_pkl('LR1.pkl')

    state_record = []
    word_record = []
    # 状态栈
    state_record.append(0)
    # 符号栈
    word_record.append("TERMINAL")

    index = 0
    while index < len(words):
        prep = []

        prep.extend(reversed(word_record))
        word_record.clear()

        # 打印并更新符号栈
        for word in reversed(prep):
            print(word)
            word_record.append(word)

        print()

        # 当前面临的符号
        top = state_record[-1]
        print(f'{top} face: {words[index]}')
        action = lr1_table[top].get(words[index])

        # 需要进行的动作
        if not action:
            print("grammer error")
            return
        print(action)

        # 得到操作
        match = re.search(r'\d+', action)
        if match:
            num = int(match.group())
        
        # 移进
        if action.startswith('s'):
            state_record.append(num)
            word_record.append(words[index])
            print(f'shift {words[index]} to {num}')
        elif action.startswith('r'):
            reduce_production(grammar, state_record, word_record, num)
            index -= 1
        # 接受
        elif action.startswith('acc'):
            print('reduce successfully')
            break

        index += 1
            
def test_grammer():
    words = ['PROGRAM', 'ID', 'CONST', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'VAR', 'ID', 'COMMA', 'ID', 'COMMA', 'ID', 'SEMI', 'BEGIN', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'WHILE', 'M', 'ID', 'LT', 'NUM', 'DO', 'M', 'ID', 'ASSIGN', 'ID', 'PLUS', 'ID', 'SEMI', 'IF', 'ID', 'NEQ', 'NUM', 'THEN', 'M', 'ID', 'ASSIGN', 'ID', 'MINUS', 'NUM', 'SEMI', 'IF', 'ID', 'LE', 'NUM', 'THEN', 'M', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'IF', 'ID', 'GE', 'NUM', 'THEN', 'M', 'ID', 'ASSIGN', 'ID', 'PLUS', 'ID', 'MINUS', 'LPAREN', 'ID', 'TIMES', 'ID', 'DIV', 'ID', 'RPAREN', 'SEMI', 'ID', 'ASSIGN', 'ID', 'PLUS', 'ID', 'SEMI', 'END', 'TERMINAL']
    test_grammer_words(words)
        

if __name__ == '__main__':
    # index = 0
    # test()
    test_grammer()