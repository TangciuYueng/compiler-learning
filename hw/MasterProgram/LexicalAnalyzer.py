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
            self.analyze_line = 1

    def error(self, message):
        raise SyntaxError(f"Lexical Error: {message}")

    def get_index(self):
        return self.analyze_index

    def analyze_once(self, ch):
        if ch == '#':
            self.analyze_index += 1
            return '#', "TERMINAL", self.analyze_line

        if ch in [' ', '\n', '\r', '\t']:
            if ch == '\n':
                self.analyze_line += 1

            if not self.analyze_stack:
                self.analyze_index += 1
                return None, None, self.analyze_line

            while self.analyze_current_state not in self.end_list:
                if not self.analyze_stack:
                    self.error(f"character \'{ch}\' at line {self.analyze_line}")

                self.analyze_current_state = self.analyze_stack.pop()
                self.analyze_index -= 1
                self.analyze_word = self.analyze_word[:-1]

            word = self.analyze_word
            state = self.analyze_current_state
            self.analyze_word = ""
            self.analyze_stack.clear()
            self.analyze_current_state = 0
            self.analyze_index += 1
            return word, regular_description_map[self.end_map[state]], self.analyze_line

        else:
            if ch not in self.tokens:
                self.error(f"character \'{ch}\' at line {self.analyze_line}")

            next_state = self.state_trans_table[self.analyze_current_state][self.tokens.index(ch) + 1]

            if next_state == -2:
                if not self.analyze_stack:
                    self.error(f"character \'{ch}\' at line {self.analyze_line}")

                while self.analyze_current_state not in self.end_list:
                    if not self.analyze_stack:
                        self.error(f"character \'{ch}\' at line {self.analyze_line}")

                    self.analyze_current_state = self.analyze_stack.pop()
                    self.analyze_index -= 1
                    self.analyze_word = self.analyze_word[:-1]

                word = self.analyze_word
                state = self.analyze_current_state
                self.analyze_word = ""
                self.analyze_stack.clear()
                self.analyze_current_state = 0
                return word, regular_description_map[self.end_map[state]], self.analyze_line
            else:
                self.analyze_current_state = next_state
                self.analyze_word += ch
                self.analyze_stack.append(self.analyze_current_state)
                self.analyze_index += 1
                return None, None, self.analyze_line