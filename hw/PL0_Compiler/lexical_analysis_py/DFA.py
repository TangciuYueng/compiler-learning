from Auto import Auto
from queue import Queue
import pickle

class DFAState:
    def __init__(self, state, num):
        # 包含的原NFA状态集合
        self.state = state
        # 在DFA中的编号
        self.num = num

class DFA(Auto):
    def __init__(self, tokens, origin_nfa_trans_list, origin_nfa_start, origin_nfa_end_list, origin_nfa_end_info_map, file_path):
        self.tokens = tokens
        self.origin_nfa_trans_list = origin_nfa_trans_list
        self.origin_nfa_start = origin_nfa_start
        self.origin_nfa_end_list = origin_nfa_end_list
        self.origin_nfa_end_info_map = origin_nfa_end_info_map

        self.start_list = set()
        self.end_list = set()
        self.detailed_state_trans_table = []
        self.state_trans_table = []
        self.end_map = {}
        self.detailed_state_map = {}

        self.file_path = file_path

        super(DFA, self).__init__()
    
    # init 为节点值
    def origin_nfa_trans(self, init, ch):
        ret = set()

        for edge in self.origin_nfa_trans_list.get_neighbor_edges(init):
            # 获得节点值
            next_val = self.origin_nfa_trans_list.get_vertex(edge.dest)
            # edge 的 cost
            rule = edge.cost

            if rule == "<letter>":
                if ch.isalpha():
                    next_val = self.origin_nfa_trans_list.get_vertex(edge.dest)
                    ret.add(next_val)
                
            elif rule == "<digit>":
                if ch.isdigit():
                    next_val = self.origin_nfa_trans_list.get_vertex(edge.dest)
                    ret.add(next_val)
            
            elif rule != "<epsilon>":
                if ch in rule:
                    ret.add(next_val)

        return ret

    def get_closure(self, origin):
        closure = set()
        close_list = set()
        open_list = Queue()

        for state in origin:
            open_list.put(state)
            closure.add(state)

        while not open_list.empty():
            u = open_list.get()

            for edge in self.origin_nfa_trans_list.get_neighbor_edges(u):     
                if edge.cost == "<epsilon>":
                    v_val = self.origin_nfa_trans_list.get_vertex(edge.dest)
                    closure.add(v_val)

                    if v_val not in close_list:
                        open_list.put(v_val)

            close_list.add(u)

        return closure

    def subset_method(self):
        # 已经出现的状态
        record_states = {}     
        # 待求结果的状态
        remain_states = Queue()

        init_state = set()
        init_state.add(self.origin_nfa_start)

        self.start_list.add(0)
        init_state = self.get_closure(init_state)

        record_states[frozenset(init_state)] = 0
        remain_states.put(DFAState(init_state, 0))
        self.detailed_state_map[0] = init_state

        while not remain_states.empty():
            line = []
            state = remain_states.get()
            line.append(state.num)

            for token in self.tokens:
                transed_state = set()
                for stat in state.state:
                    for s in self.origin_nfa_trans(stat, token):
                        transed_state.add(s)

                transed_state = self.get_closure(transed_state)

                if frozenset(transed_state) in record_states:
                    num = record_states[frozenset(transed_state)]
                    line.append(num)
                else:
                    if len(transed_state) == 0:
                        line.append(-2)
                    else:
                        num = len(record_states)
                        remain_states.put(DFAState(transed_state, num))
                        record_states[frozenset(transed_state)] = num
                        line.append(num)

                        self.detailed_state_map[num] = transed_state

                        if self.origin_nfa_start in transed_state:
                            self.start_list.add(num)
                        else:
                            inter = transed_state.intersection(self.origin_nfa_end_list)

                            if len(inter) != 0:
                                self.end_list.add(num)
                                min_priority = float('inf')
                                selected_regular = ''

                                for inter_state in inter:
                                    info = self.origin_nfa_end_info_map[inter_state]

                                    if min_priority > info.priority:
                                        min_priority = info.priority
                                        selected_regular = info.regular
                                    
                                self.end_map[num] = selected_regular

            self.state_trans_table.append(line)

    def show_state_trans_table(self):
        print("    ", end="")
        for token in self.tokens:
            print(f"{token:4}", end="")
        print()

        for line in self.state_trans_table:
            for state in line:
                if state != -2:
                    print(f"{state:4}", end="")
                else:
                    print("    ", end="")
            print()

    def save_to_file(self):
        with open(self.file_path, 'wb') as file:
            pickle.dump(self.tokens, file)
            pickle.dump(self.state_trans_table, file)
            pickle.dump(self.end_list, file)
            pickle.dump(self.end_map, file)
            print('保存成功！')