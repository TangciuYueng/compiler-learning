from enum import Enum
from queue import Queue

class TransType(Enum):
    ACTION_SHIFT = 1
    ACTION_REDUCE = 2
    ACTION_ACC = 3
    GOTO = 4

class Trans:
    def __init__(self, trans_type, dest=-1):
        self.type = trans_type
        self.dest = dest

    def format(self):
        if self.type == TransType.ACTION_SHIFT:
            return f"s{self.dest}"
        elif self.type == TransType.ACTION_REDUCE:
            return f"r{self.dest}"
        elif self.type == TransType.ACTION_ACC:
            return "acc"
        elif self.type == TransType.GOTO:
            return str(self.dest)
        else:
            return ""

class DFAState:
    def __init__(self, num=-1, productions=None):
        self.num = num
        self.productions = productions or []

    def __str__(self):
        result = f"DFAState {self.num}:\n"
        result += "\n".join([str(production) for production in self.productions])
        return result

    def __repr__(self):
        return self.__str__()

from Production import Element, Production, DFAProduction

class DFA:
    def __init__(self, grammar, terminals, nonterminals, start_word):
        # 文法起始符
        self.start_word = start_word
        # str -> set<int> 产生式左部到以该词为左部产生式编号的映射
        self.production_left_ref = {}
        # Production -> num 产生式到编号的映射
        self.production_to_num = {}
        # num -> Production 包含一个列表的多个 Element
        self.num_to_production = {}
        # str -> set<str> 各个词的FIRST
        self.first_list = {}
        # list<map<str, set<Trans>>> LR1分析表
        self.table_lr1 = []
        
        self.pre_productions(grammar)
        self.pre_word(terminals, nonterminals)

    # 文法中的产生式进行预处理
    def pre_productions(self, grammar):
        num = 0

        for production in grammar:
            # 加入 产生式 - 编号 映射表
            self.production_to_num[production] = num
            self.num_to_production[num] = production

            # 左部加入
            place = self.production_left_ref.get(production.left, None)

            if place is not None:
                place.add(num)
            else:
                new_set = {num}
                self.production_left_ref[production.left] = new_set

            num += 1

    # 求取 FIRST 集合
    def pre_word(self, terminals, nonterminals):
        for terminal in terminals:
            self.form_first(Element(terminal, True))

        for nonterminal in nonterminals:
            self.form_first(Element(nonterminal, False))

    def form_first(self, element):
        if element.if_terminal:
            self.first_list[element.word] = {element.word}
        else:
            self.form_first_recursive(element)

    # 递归求非终结符的 FIRST 集合
    def form_first_recursive(self, element):
        # 终结符返回自己
        if element.if_terminal:
            return {element.word}

        # 非终结符如果已经求出也返回
        if element.word in self.first_list:
            return self.first_list[element.word]

        # 待求的 FIRST 集合
        new_set = set()

        # 以该词为左部的产生式集合
        for pro_num in self.production_left_ref[element.word]:
            
            production = self.num_to_production[pro_num]
                
            iter1 = iter(production.right)

            while True:
                try:
                    current_element = next(iter1)
                except StopIteration:
                    new_set.add("EPSILON")
                    break

                if current_element.if_terminal:
                    new_set.add(current_element.word)
                    break

                if current_element.word == element.word:
                    break

                check_set = self.form_first_recursive(current_element)

                if "EPSILON" in check_set:
                    check_set.remove("EPSILON")
                    new_set.update(check_set)
                else:
                    new_set.update(check_set)
                    break


        self.first_list[element.word] = new_set
        return new_set

    # 查询一个词的 FIRST 传入参数为 Element
    def get_first(self, word):
        pair = self.first_list.get(word.word)

        if pair is None:
            return set()

        return pair

    # 查询一个句子的 FIRST 集合 传入参数为 List<Element>
    def get_first_for_word_list(self, word_list):
        if not word_list:
            return set()

        if len(word_list) == 1:
            return self.get_first(word_list[0])

        ret = set()
        for word in word_list:
            if word.if_terminal:
                ret.add(word.word)
                return ret

            check_set = self.get_first(word)
            if "EPSILON" in check_set:
                check_set.remove("EPSILON")
                ret.update(check_set)
            else:
                ret.update(check_set)
                return ret

        ret.add("EPSILON")
        return ret

    # 打印所有 FIRST 集合
    def show_first_list(self):
        for key, value in self.first_list.items():
            print(f"FIRST({key}) = {value}")

    # 求给定 LR1 项目集的闭包 传入 DFAState
    def get_closure(self, state):
        # 遍历 state 里面的每个产生式
        i = 0
        while True:
            # print(i)
            # print(len(state.productions))
            if i >= len(state.productions):
                break

            # 规约项目 只能接收终结符
            if state.productions[i].end() or state.productions[i].get_point().if_terminal:
                i += 1
                continue

            new_outlook = set()
            next_point = state.productions[i].point + 1

            # 圆点到头
            if state.productions[i].next_end():
                new_outlook.update(state.productions[i].outlook)
            # 没到头
            else:
                for outlook in state.productions[i].outlook:
                    part = state.productions[i].right[next_point:]
                    part.append(Element(outlook, True))

                    part_first = self.get_first_for_word_list(part)
                    # new_outlook.update(part_first)
                    new_outlook = part_first.union(new_outlook)

                new_outlook.discard("EPSILON")

            left = state.productions[i].get_point().word
            for pro_num in self.production_left_ref[left]:
                production = self.num_to_production[pro_num]
                new_dp = DFAProduction(production.left, production.right, new_outlook)
                if_this_add = all(new_dp != dp for dp in state.productions)

                if if_this_add:
                    state.productions.append(new_dp)

            i += 1

    def lr1(self):
        # 初始状态
        init_state = DFAState()
        # 扩展列表
        open_list = Queue()
        # 现有状态集
        # exist_list = {}
        exist_list = []

        # Add the LR(1) project with the starting symbol as the left part of the production and the outlook symbol "TERMINAL"
        production = self.num_to_production[0]
        init_state.num = 0
        init_outlook = {"TERMINAL"}
        init_state.productions.append(DFAProduction(production.left, production.right, init_outlook))
        self.get_closure(init_state)
        # print('init state prods', init_state)

        # exist_list[tuple(init_state.productions)] = 0
        exist_list.append(init_state.productions)
        self.table_lr1.append({})  # Add a new row to the LR(1) analysis table
        open_list.put(init_state)

        cnt = 0

        while not open_list.empty():
            # if cnt > 30:
            #     break
            current = open_list.get()
            # 当前状态能转移到的所有状态集合
            prep_open_list = {}

            # 遍历当前所有产生式
            for dfa_production in current.productions:
                # 圆点到头忽略
                if dfa_production.end():
                    continue

                # 要接收的符号
                recv = dfa_production.get_point()
                # 点后移
                dfa_production.add()
                
                # print('recv', recv)
                dfa_production_copy = DFAProduction(dfa_production.left, dfa_production.right, dfa_production.outlook, dfa_production.point)

                if recv in prep_open_list:
                    # prep_open_list[recv].productions.append(dfa_production)
                    prep_open_list[recv].productions.append(dfa_production_copy)
                else:
                    # new_productions = [dfa_production]
                    new_productions = [dfa_production_copy]
                    new_state = DFAState(-1, new_productions)
                    prep_open_list[recv] = new_state

                # print('prep open list', len(prep_open_list))

            for edge, state in prep_open_list.items():
                # print('state before', state)
                self.get_closure(state)
                # print('state after', state)
                
                # print(exist_list)
                # first_key = next(iter(exist_list))
                # print(first_key)
                # print(tuple(current.productions))
                # print(first_key == tuple(current.productions))
                # print(current)
                # start_pos = exist_list[tuple(current.productions)]
                start_pos = exist_list.index(current.productions)
                # end = exist_list.get(tuple(state.productions))
                end = None
                for i, productions in enumerate(exist_list):
                    if set(productions) == set(state.productions):
                        print(productions)
                        print(state.productions)
                        end = i

                if not end:
                    print('\n\n')
                    print(exist_list)
                    print(state.productions)
                # try:
                #     end = exist_list.index(state.productions)
                #     print('\na')
                # except ValueError:
                #     end = None
                    # print('\nb',exist_list)
                # 状态在当前 DFA 中存在
                if end is not None:
                    self.add_table_lr1_edge(start_pos, end, edge)
                    if state.num == 0:
                        self.add_table_lr1_vertex(state)
                # 状态在当前 DFA 中不存在
                else:
                    state.num = len(exist_list)
                    # print("New State:", state.num)
                    # exist_list[tuple(state.productions)] = state.num
                    # print('state productions before insert', state.productions)
                    exist_list.append(state.productions)
                    open_list.put(state)
                    self.table_lr1.append({})
                    self.add_table_lr1_edge(start_pos, state.num, edge)
                    self.add_table_lr1_vertex(state)
            cnt += 1

    def add_table_lr1_edge(self, start, end, word):
        trans_place = self.table_lr1[start].get(word.word, set())

        if trans_place:
            trans_place.add(Trans(TransType.ACTION_SHIFT if word.if_terminal else TransType.GOTO, end))
        else:
            new_set = {Trans(TransType.ACTION_SHIFT if word.if_terminal else TransType.GOTO, end)}
            self.table_lr1[start][word.word] = new_set

    def add_table_lr1_vertex(self, state):
        for dfa_production in state.productions:
            if dfa_production.end():
                production = Production(dfa_production.left, dfa_production.right)
                p_num = self.production_to_num[production]

                if dfa_production.left == self.start_word:
                    new_set = {Trans(TransType.ACTION_ACC, p_num)}
                    self.table_lr1[state.num]["TERMINAL"] = new_set
                else:
                    for outlook in dfa_production.outlook:
                        trans_place = self.table_lr1[state.num].get(outlook, set())

                        if trans_place:
                            trans_place.add(Trans(TransType.ACTION_REDUCE, p_num))
                        else:
                            new_set = {Trans(TransType.ACTION_REDUCE, p_num)}
                            self.table_lr1[state.num][outlook] = new_set

    def format_trans(self, trans_set):
        ret = ""
        for trans in trans_set:
            ret += '/' + trans.format() if ret else trans.format()
        return ret

    def show_table_lr1(self, terminals, nonterminals):
        dup_states = set()
        width = 10

        print("   ", end="")
        for terminal in terminals:
            print(f"{terminal: <{width}}", end="")
        for nonterminal in nonterminals:
            print(f"{nonterminal: <{width}}", end="")
        print()

        for i in range(len(self.table_lr1)):
            print(f"{i: <4}", end="")

            for terminal in terminals:
                trans_place = self.table_lr1[i].get(terminal, set())
                if trans_place:
                    if len(trans_place) != 1:
                        dup_states.add(i)
                    print(f"{self.format_trans(trans_place): <{width}}", end="")
                else:
                    print(" " * width, end="")

            for nonterminal in nonterminals:
                trans_place = self.table_lr1[i].get(nonterminal, set())
                if trans_place:
                    if len(trans_place) != 1:
                        dup_states.add(i)
                    print(f"{self.format_trans(trans_place): <{width}}", end="")
                else:
                    print(" " * width, end="")

            print()

        print("-------------------------------")
        print(" ".join(map(str, dup_states)))
        print("-------------------------------")

def run_dfa_test():
    # Define a simple grammar
    grammar = [
        Production('S', [Element('E', False)]),
        # Production('E', [Element('E', False), Element('+', True), Element('T', False)]),
        Production('E', [Element('T', False)]),
        # Production('T', [Element('T', False), Element('*', True), Element('F', False)]),
        Production('T', [Element('F', False)]),
        Production('F', [Element('(', True), Element('E', False), Element(')', True)]),
        # Production('F', [Element('id', True)])
    ]

    terminals = {'+', '*', '(', ')', 'id'}
    nonterminals = {'S', 'E', 'T', 'F'}
    start_word = 'S'

    dfa = DFA(grammar, terminals, nonterminals, start_word)
    # dfa.show_first_list()
    dfa.lr1()

    # Show LR(1) analysis table
    # dfa.show_table_lr1(terminals, nonterminals)

if __name__ == "__main__":
    run_dfa_test()