from NFA import *
from RegularCalculator import RegularCalculator
from DFA import DFA

def reset_bracket(string):
    return ''.join(['(' if char == '[' else (')' if char == ']' else char) for char in string])

regular = 'PROGRAM|BEGIN|END|CONST|VAR|WHILE|DO|IF|THEN|l(l|d)`|dd`|+|-|*|/|:=|=|<>|>|>=|<|<=|[|]|;|,'

tokens = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz()+-*/:=<>;,'

from re import split
regulars = split(r'\|(?![^\(]*\))', regular)

# print(regulars)
nfas = []
total_start_list = set()
total_end_list = set()
total_end_info_map = {}

for i, origin in enumerate(regulars):
    regulars[i] = RegularCalculator.PostfixPrep(regulars[i])
    regulars[i] = RegularCalculator.Postfix(regulars[i])
    regulars[i] = reset_bracket(regulars[i])
    # print(regulars[i])
    
    nfa = NFA()
    RegularCalculator.Calculation(regulars[i], nfa)
    nfas.append(nfa)

    nfa_info = nfa.get_info()
    total_start_list.add(nfa_info.start)
    total_end_list.add(nfa_info.end)

    origin = reset_bracket(origin)
    total_end_info_map[nfa_info.end] = NFAEndInfo(origin, i)

new_start = NFA.state_count
NFA.state_count += 1
for start in total_start_list:
    NFA.total_trans_list.append(NFATrans(new_start, start, "<epsilon>"))

total_NFA = NFA()
total_NFA.make_total_nfa(new_start, total_end_list, total_end_info_map)
# total_NFA.show()

file_path = './lexical.pkl'

dfa = DFA(tokens, total_NFA.trans_list, total_NFA.get_info().start, total_NFA.end_list, total_NFA.end_info_map, file_path)
dfa.subset_method()
dfa.show_state_trans_table()
# dfa.save_to_file()