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

import re
def test_grammer(words):
    global symbols, lr1_table
    read_from_pkl('LR1.pkl')

    state_record = []
    word_record = []
    state_record.append(0)
    word_record.append("TERMINAL")

    index = 0
    while index < len(words):
        prep = []
        while len (word_record) != 0:
            prep.append(word_record.pop())

        it = len(prep) - 1
        while it >= 0:
            print(prep[it], ' ')
            word_record.append(prep[it])
            it -= 1

        print()

        top = state_record[-1]
        print(f'{top} face: {words[index]}')
        action = lr1_table[top].get(words[index])

        if not action:
            print("grammer error")
            return
        
        print(action)
        match = re.search(r'\d+', action)
        num = int(match.group())
        if action.startswith('s'):
            state_record.append(num)
            word_record.append(words[index])
            print(f'shift {words} to {num}')
        elif action.startswith('r'):
            # get the Number num production
            production = get_production[num]
            left = production.left
            reduce_length = len(production.right)
            if reduce_length == 1 and production.right[0].word == 'EPSILON':
                reduce_length = 0
            for _ in range(reduce_length):
                state_record.pop()
                word_record.pop()
            new_top = state_record[-1]
            new_action = lr1_table[new_top].get(words[index])
            new_match = re.search(r'\d+', new_action)
            new_num = int(match.group())
            state_record.append(new_num)
            word_record.append(words[index])
            print(f'reduce with Number {num} production to {state_record[-1]}')
            index -= 1
        elif action.startswith('acc'):
            print('reduce successfully')
            return

        index += 1
            
        

        

if __name__ == '__main__':
    # index = 0
    # test()
