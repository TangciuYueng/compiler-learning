import pickle
import re

import intermediate
from Production import grammar
from intermediate import visit_functions


class GrammarAnalyzer:
    def __init__(self, file_path):
        with open(file_path, 'rb') as file:
            self.symbols = pickle.load(file)
            self.lr1_table = pickle.load(file)
            self.index = 0
            self.state_record = [0]
            self.word_record = [('#', 'TERMINAL')]

    def error(self, message):
        raise SyntaxError(f"Grammar Error: {message}")

    def reduce_production(self, num, line):
        intermediate.set_line(line)
        production = grammar[num]
        left = production.left
        reduce_length = len(production.right)

        if reduce_length == 1 and production.right[0].word == 'EPSILON':
            reduce_length = 0

        right = []
        for _ in range(reduce_length):
            self.state_record.pop()
            right.append(self.word_record.pop())

        new_top = self.state_record[-1]
        new_action = self.lr1_table[new_top].get(left)
        new_match = re.search(r'\d+', new_action)
        new_num = int(new_match.group())
        self.state_record.append(new_num)
        self.word_record.append(left)

        right.reverse()
        if visit_functions[num]:
            visit_functions[num](right, self.word_record)

    def analyze_once(self, word, element, line):
        top = self.state_record[-1]
        action = self.lr1_table[top].get(element)

        if not action:
            self.error(f"word \"{word}\" at line {line}")

        match = re.search(r'\d+', action)  # 匹配r开头的动作，即规约动作
        num = None
        if match:
            num = int(match.group())  # 若匹配成功，取得产生式编号

        # 移进
        if action.startswith('s'):
            self.state_record.append(num)
            self.word_record.append((word, element))
            return True, True, None
        # 规约
        elif action.startswith('r'):
            self.reduce_production(num, line)
            return True, False, num
        # 接受
        elif action.startswith('acc'):
            return False, True, None