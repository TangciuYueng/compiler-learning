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
            self.analyze_stack = deque()
            self.analyze_current_state = 0
            self.analyze_word = ""
            self.analyze_index = 0

    def error(self, message):
        raise SyntaxError(f"Lexical Error: {message}")

    def get_index(self):
        return self.analyze_index

    def ch_format(self, ch):
        if ch == ' ':
            return 'space'
        elif ch == '\n':
            return '\\n'
        elif ch == '\r':
            return '\\r'
        elif ch == '\t':
            return '\\t'
        else:
            return ch

    def analyze_once(self, ch, current_line):
        if_nextline = False

        if ch == '#':
            self.analyze_index += 1
            return '#', "TERMINAL", if_nextline

        if ch in [' ', '\n', '\r', '\t']:
            if ch == '\n':
                if_nextline = True

            if not self.analyze_stack:
                self.analyze_index += 1
                return None, None, if_nextline

            while self.analyze_current_state not in self.end_list:
                if not self.analyze_stack:
                    self.error(f"character \'{self.ch_format(ch)}\' at line {current_line}")

                self.analyze_current_state = self.analyze_stack.pop()
                self.analyze_index -= 1
                self.analyze_word = self.analyze_word[:-1]

            word = self.analyze_word
            state = self.analyze_current_state
            self.analyze_word = ""
            self.analyze_stack.clear()
            self.analyze_current_state = 0
            self.analyze_index += 1
            return word, regular_description_map[self.end_map[state]], if_nextline

        else:
            if ch not in self.tokens:
                self.error(f"character \'{self.ch_format(ch)}\' at line {current_line}")

            next_state = self.state_trans_table[self.analyze_current_state][self.tokens.index(ch) + 1]

            if next_state == -2:
                if not self.analyze_stack:
                    self.error(f"character \'{self.ch_format(ch)}\' at line {current_line}")

                while self.analyze_current_state not in self.end_list:
                    if not self.analyze_stack:
                        self.error(f"character \'{self.ch_format(ch)}\' at line {current_line}")

                    self.analyze_current_state = self.analyze_stack.pop()
                    self.analyze_index -= 1
                    self.analyze_word = self.analyze_word[:-1]

                word = self.analyze_word
                state = self.analyze_current_state
                self.analyze_word = ""
                self.analyze_stack.clear()
                self.analyze_current_state = 0
                return word, regular_description_map[self.end_map[state]], if_nextline
            else:
                self.analyze_current_state = next_state
                self.analyze_word += ch
                self.analyze_stack.append(self.analyze_current_state)
                self.analyze_index += 1
                return None, None, if_nextline