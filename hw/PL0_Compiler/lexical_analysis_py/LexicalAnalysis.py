import pickle
from collections import deque

regular_description_map = {
    "PROGRAM": "PROGRAM",
    "BEGIN": "BEGIN",
    "END": "END",
    "CONST": "CONST",
    "VAR": "VAR",
    "WHILE": "WHILE",
    "DO": "DO",
    "IF": "IF",
    "THEN": "THEN",
    "l(l|d)`": "ID",
    "dd`": "NUM",
    "+": "PLUS",
    "-": "MINUS",
    "*": "TIMES",
    "/": "DIV",
    ":=": "ASSIGN",
    "=": "EQUAL",
    "<>": "NEQ",
    ">": "GT",
    ">=": "GE",
    "<": "LT",
    "<=": "LE",
    "(": "LPAREN",
    ")": "RPAREN",
    ";": "SEMI",
    ",": "COMMA",
    }

class LexicalAnalyzer:

    def __init__(self, file_path):
        with open(file_path, 'rb') as file:
            self.tokens = pickle.load(file)
            self.state_trans_table = pickle.load(file)
            self.end_list = pickle.load(file)
            self.end_map = pickle.load(file)

    def handle_separator(self, stack, current, index, word):
        while current not in self.end_list:
            if not stack:
                raise ValueError("Unexpected error during separator handling.")
            current = stack.pop()
            index -= 1
            word = word[:-1]
        print(regular_description_map[self.end_map[current]])

        if word in ["WHILE", "DO", "THEN"]:
            print("M")

        return "", []

    def analyze1(self, input_program):
        stack = deque()
        current_state = 0
        word = ""
        index = 0

        for current_char in input_program:
            if current_char in [' ', '\n', '\r']:
                if not stack:
                    continue
                word, stack = self.handle_separator(stack, current_state, index, word)
            else:
                if current_char not in self.tokens:
                    raise ValueError("Illegal character encountered.")
                
                next_state = self.state_trans_table[current_state][self.tokens.index(current_char)]

                if next_state == -2:
                    if not stack:
                        raise ValueError("Unexpected error.")
                    
                    word, stack = self.handle_separator(stack, current_state, index, word)
                    index -= 1

                else:
                    current_state = next_state
                    word += current_char
                    stack.append(current_state)

            index += 1

        if stack:
            raise ValueError("Unexpected end of input.")

        print("TERMINAL")
        return True

    def analyze(self, input_program):
        stack = deque()
        current_state = 0
        word = ""
        index = 0

        while index < len(input_program):
            if input_program[index] in [' ', '\n', '\r', '\t']:
                if not stack:
                    index += 1
                    continue
                while current_state not in self.end_list:
                    if not stack:
                        print("a")
                        return False
                    
                    current_state = stack.pop()
                    index -= 1
                    word = word[:-1]

                print(word, regular_description_map[self.end_map[current_state]])

                if word in ["WHILE", "DO", "THEN"]:
                    print("M")

                word = ""
                stack = deque()
                current_state = 0

            else:
                if input_program[index] not in self.tokens:
                    print('B')
                    return False
                
                next_state = self.state_trans_table[current_state][self.tokens.index(input_program[index]) + 1]

                if next == -2:
                    if not stack:
                        print("c")
                        return False

                    while current_state not in self.end_list:
                        if not stack:
                            print('d')
                            return False
                        current_state = stack.pop()
                        index -= 1
                        word = word[:-1]
                
                    print(word, regular_description_map[self.end_map[current_state]])

                    if word in ["WHILE", "DO", "THEN"]:
                        print("M")
                    word = ''
                    stack = deque()
                    current_state = 0
                    index -= 1

                else:
                    current_state = next_state
                    word += input_program[index]
                    stack.append(current_state)
            index += 1

def test():
    file_path = './lexical.pkl'
    input_program = (
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
    analyzer = LexicalAnalyzer(file_path)
    analyzer.analyze(input_program)

if __name__ == '__main__':
    test()
    