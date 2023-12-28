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
    '+': 'PLUS', 
    '-': 'MINUS', 
    '*': 'TIMES', 
    '/': 'DIV', 
    '=': 'EQUAL', 
    '(': 'LPAREN', 
    ')': 'RPAREN', 
    ';': 'SEMI', 
    ',': 'COMMA'
}

def error(message):
    raise ValueError(f"Lexical error: {message}")

def next(index):
    while index < len(src):
        if src[index] in ['\n', ' ', '\t', '\r']:
            index += 1
            
        # 标识符
        elif src[index].islower():
            word = ''
            while index < len(src) and (src[index].islower() or src[index].isdigit()):
                word += src[index]
                index += 1
            
            return (word, 'ID'), index
        
        # 数字
        elif src[index].isdigit():
            num = 0
            while index < len(src) and src[index].isdigit():
                num = num * 10 + int(src[index])
                index += 1
            
            return (num, 'NUM'), index

        elif src[index] == ':':
            index += 1
            if index < len(src) and src[index] == '=':
                index += 1
                return (":=", 'ASSIGN'), index
            else:
                error("Expected '=' after ':'")

        elif src[index] == '<':
            index += 1
            if index < len(src) and src[index] == '=':
                index += 1
                return ('<=', 'LE'), index
            elif index < len(src) and src[index] == '>':
                index += 1
                return ('<>', 'NEQ'), index
            else:
                return ('<', 'LT'), index
        
        elif src[index] == '>':
            index += 1
            if index < len(src) and src[index] == '=':
                index += 1
                return ('>=', 'GE'), index
            else:
                return ('>', 'GT'), index
        
        elif src[index] in single_symbol.keys():
            index += 1
            return (src[index - 1], single_symbol[src[index - 1]]), index

        elif src[index].isupper():
            keyword_found = False
            for keyword in ["PROGRAM", "BEGIN", "END", "CONST", "VAR", "WHILE", "DO", "IF", "THEN"]:
                if src[index:].startswith(keyword):
                    index += len(keyword)
                    return (keyword, keyword), index
                    keyword_found = True

            if not keyword_found:
                error("Invalid keyword")

        else:
            error("Invalid character")
    return ('TERMINAL', 'TERMINAL'), index

src = (
        "PROGRAM add\n"
        "CONST sb := 1000;\n"
        "VAR x1, y, helloworld;\n"
        "BEGIN\n"
        "   x1 := 1;\n"
        "   y := 2;\n"
        "   helloworld := 65536;\n"
        "   WHILE x1<5 DO x := x1+ (helloworld+2);\n"
        "   IF y <> 0 THEN y := y- (y+x1);\n"
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
            lex, index = next(index)
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
    Production("CompoundStatement", [Element("BEGIN", True), Element("LastStatement", False), Element("END", True)]),
    Production("LastStatement", [
        Element("LastStatement", False),
        Element("SEMI", True),
        Element("M", False),
        Element("Statement", False)
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

from Counter import *

temp_var_counter = TempVarCounter()
mid_code_counter = MidCodeCounter(100)
condition_counter = ConditionCounter()
next_counter = NextCounter()
mid_codes = {}
true_lists = {}
false_lists = {}
next_lists = {}
existing_symbols = {}

def visit_constant_definition(node, word_record):
    # ConstantDefinition -> ID ASSIGN NUM
    if node[0] in existing_symbols:
        error(f'{node[0][0]} has been declared')
    else:
        existing_symbols[node[0][0]] = TokenType.CONST
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{node[0][0]} := {node[2][0]}'

def visit_variable_declaration(node, word_record):
    # VariableDeclaration -> VAR ID NextID
    if node[1] in existing_symbols:
        error(f'{node[1]} has been declared')
    else:
        existing_symbols[node[1]] = TokenType.VAR


def visit_next_id(node, word_record):
    # NextID -> COMMA ID NextID
    if node[1][0] in existing_symbols:
        error(f'{node[1]} has been declared')
    else:
        existing_symbols[node[1]] = TokenType.VAR

def visit_compound_statement(node, word_record):
    # CompoundStatement -> BEGIN LastStatement Statement END
    next_cnt = node[1]
    new_next_list = list(next_lists.get(next_cnt, []))

    new_next_cnt = next_counter.get_next_counter()
    next_lists[new_next_cnt] = new_next_list

    word_record.pop()
    word_record.append(new_next_cnt)

def visit_last_statement(node, word_record):
    # LastStatement -> LastStatement SEMI M Statement
    next_cnt = node[0]
    m = node[2]

    # 回填
    next_list = next_lists.get(next_cnt)
    if next_list:
        for num in next_list:
            mid_code = mid_codes.get(num, "")
            mid_code = f'{mid_code}{m}'
            mid_codes[num] = mid_code

    # next_list 往前传
    statement_next_cnt = node[3]
    new_next_cnt = next_counter.get_next_counter()
    next_lists[new_next_cnt] = list(next_lists.get(statement_next_cnt, []))

    word_record.pop()
    word_record.append(new_next_cnt)

def visit_last_statement_single(node, word_record):
    # LastStatement -> Statement
    next_cnt = node[0]
    new_next_list = next_lists.get(next_cnt, [])

    new_next_cnt = next_counter.get_next_counter()

    next_lists[new_next_cnt] = new_next_list

    word_record.pop()
    word_record.append(new_next_cnt)

def visit_statement_assignment(node, word_record):
    # Statement -> AssignmentStatement
    word_record.pop()
    word_record.append(node[0])

def visit_statement_condition(node, word_record):
    # Statement -> ConditionStatement
    word_record.pop()
    word_record.append(node[0])

def visit_statement_loop(node, word_record):
    # Statement -> LoopStatement
    word_record.pop()
    word_record.append(node[0])

def visit_statement_compound(node, word_record):
    # Statement -> CompoundStatement
    word_record.pop()
    word_record.append(node[0])

def visit_statement_empty(node, word_record):
    # Statement -> EmptyStatement
    pass

def visit_assignment_statement(node, word_record):
    # AssignmentStatement -> Identifier ASSIGN Expression
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{node[0]} := {node[2]}'

    next_cnt = next_counter.get_next_counter()
    next_lists[next_cnt] = []

    word_record.pop()
    word_record.append(next_cnt)

def visit_expression_plus_term(node, word_record):
    # Expression -> PLUS Term
    res = temp_var_counter.new_temp_var()
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{res} = {node[1]}'
    word_record.pop()
    word_record.append(res)

def visit_expression_minus_term(node, word_record):
    # Expression -> MINUS Term
    res = temp_var_counter.new_temp_var()
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{res} = - {node[1]}'
    word_record.pop()
    word_record.append(res)

def visit_expression_term(node, word_record):
    # Expression -> Term
    word_record.pop()
    word_record.append(node[0])

def visit_expression_e_p_t(node, word_record):
    # Expression -> Expression PLUS Term
    res = temp_var_counter.new_temp_var()
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{res} = {node[0]} + {node[2]}'
    word_record.pop()
    word_record.append(res)

def visit_expression_e_m_t(node, word_record):
    # Expression -> Expression MINUS Term
    res = temp_var_counter.new_temp_var()
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{res} = {node[0]} - {node[2]}'
    word_record.pop()
    word_record.append(res)

def visit_term_factor(node, word_record):
    # Term -> Factor
    word_record.pop()
    word_record.append(node[0])

def visit_term_t_t_f(node, word_record):
    # Term -> Term TIMES Factor
    res = temp_var_counter.new_temp_var()
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{res} = {node[0]} * {node[2]}'
    word_record.pop()
    word_record.append(res)

def visit_term_t_d_f(node, word_record):
    # Term -> Term DIV Factor
    res = temp_var_counter.new_temp_var()
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{res} = {node[0]} / {node[2]}'
    word_record.pop()
    word_record.append(res)

def visit_factor_i(node, word_record):
    # Factor -> Identifier
    word_record.pop()
    word_record.append(node[0])

def visit_factor_n(node, word_record):
    # Factor -> NUM
    word_record.pop()
    word_record.append(node[0][0])

def visit_factor_l_e_r(node, word_record):
    # Factor -> LPAREN Expression RPAREN
    word_record.pop()
    word_record.append(node[1])

def visit_condition_statement(node, word_record):
    # ConditionStatement -> IF Condition THEN M Statement
    cond_cnt = node[1]
    m = node[3]
    next_cnt = node[4]

    true_list = true_lists.get(cond_cnt)
    if true_list:
        for num in true_list:
            mid_code = mid_codes.get(num, "")
            mid_code = f'{mid_code}{m}'
            mid_codes[num] = mid_code

    # 更新 nextList 为 falseList + statementNextList
    new_next_cnt = next_counter.get_next_counter()
    new_next_list = list(false_lists.get(cond_cnt, [])) + list(next_lists.get(next_cnt, []))
    next_lists[new_next_cnt] = new_next_list

    word_record.pop()
    word_record.append(new_next_cnt)

def visit_m(node, word_record):
    # M -> EPSILON
    word_record.pop()
    word_record.append(mid_code_counter.get_current_mid_code_counter())

def visit_loop_statement(node, word_record):
    # LoopStatement -> WHILE M Condition DO M Statement
    
    # 条件的入口地址
    m1 = node[1]
    # true_list 和 false_list 的索引
    cond_cnt = node[2]
    # 语句的入口地址
    m2 = node[4]
    # 下一个语句索引
    next_cnt = node[5]

    # m2 to E.true_list
    true_list = true_lists.get(cond_cnt)
    if true_list:
        for num in true_list:
            mid_code = mid_codes.get(num, "")
            mid_code = f"{mid_code}{m2}"
            mid_codes[num] = mid_code
    
    # m1 to S1.next_list
    next_list = next_lists.get(next_cnt)
    if next_list:
        for num in next_list:
            mid_code = mid_codes.get(num, "")
            mid_code = f"{mid_code}{m1}"
            mid_codes[num] = mid_code

    # S.next_list = E.false_list
    new_next_list = list(false_lists.get(cond_cnt, []))
    new_next_cnt = next_counter.get_next_counter()
    next_lists[new_next_cnt] = new_next_list

    # emit j, -, -, m1
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'goto {m1}'

    word_record.pop()
    word_record.append(new_next_cnt)

def visit_condition(node, word_record):
    # Condition -> Expression RelationalOperator Expression
    cnt1 = mid_code_counter.get_mid_code_counter()
    true_list = [cnt1]
    cnt2 = mid_code_counter.get_mid_code_counter()
    false_list = [cnt2]

    cond_cnt = condition_counter.get_condition_counter()
    true_lists[cond_cnt] = true_list
    false_lists[cond_cnt] = false_list

    mid_codes[cnt1] = f'if {node[0]} {node[1]} {node[2]} goto '
    mid_codes[cnt2] = 'goto '

    word_record.pop()
    word_record.append(cond_cnt)

def visit_relop_eq(node, word_record):
    # RelationalOperator -> EQ
    word_record.pop()
    word_record.append(node[0][0])

def visit_relop_neq(node, word_record):
    # RelationalOperator -> NEQ
    word_record.pop()
    word_record.append(node[0][0])

def visit_relop_lt(node, word_record):
    # RelationalOperator -> LT
    word_record.pop()
    word_record.append(node[0][0])

def visit_relop_le(node, word_record):
    # RelationalOperator -> LE
    word_record.pop()
    word_record.append(node[0][0])

def visit_relop_gt(node, word_record):
    # RelationalOperator -> GT
    word_record.pop()
    word_record.append(node[0][0])

def visit_relop_ge(node, word_record):
    # RelationalOperator -> GE
    word_record.pop()
    word_record.append(node[0][0])

def visit_empty_statement(node, word_record):
    pass
    # EmptyStatement -> EPSILON

def visit_identifier(node, word_record):
    # Identifier -> ID
    word_record.pop()
    word_record.append(node[0][0])


visit_functions = [
    None, # Program -> Program
    None, # Program -> PROGRAM SubProgram
    None, # SubProgram -> ConstantDeclaration VariableDeclaration Statement
    None, # SubProgram -> ConstantDeclaration Statement
    None, # SubProgram -> VariableDeclaration Statement
    None, # SubProgram -> Statement
    None, # ConstantDeclaration -> CONST ConstantDefinition NextConstantDefinition
    None, # NextConstantDefinition -> COMMA ConstantDefinition NextConstantDefinition
    None, # NextConstantDefinition -> SEMI
    visit_constant_definition, # ConstantDefinition -> ID ASSIGN NUM
    visit_variable_declaration, # VariableDeclaration -> VAR ID NextID
    visit_next_id, # NextID -> COMMA ID NextID
    None, # NextID -> SEMI
    visit_compound_statement, # CompoundStatement -> BEGIN LastStatement Statement END
    visit_last_statement, # LastStatement -> LastStatement SEMI M Statement
    visit_last_statement_single, # LastStatement -> Statement
    visit_statement_assignment, # Statement -> AssignmentStatement
    visit_statement_condition, # Statement -> ConditionStatement
    visit_statement_loop, # Statement -> LoopStatement
    visit_statement_compound, # Statement -> CompoundStatement
    visit_statement_empty, # Statement -> EmptyStatement
    visit_assignment_statement, # AssignmentStatement -> Identifier ASSIGN Expression
    visit_expression_plus_term, # Expression -> PLUS Term
    visit_expression_minus_term, # Expression -> MINUS Term
    visit_expression_term, # Expression -> Term
    visit_expression_e_p_t, # Expression -> Expression PLUS Term
    visit_expression_e_m_t, # Expression -> Expression MINUS Term
    visit_term_factor, # Term -> Factor
    visit_term_t_t_f, # Term -> Term TIMES Factor
    visit_term_t_d_f, # Term -> Term DIV Factor
    visit_factor_i, # Factor -> Identifier
    visit_factor_n, # Factor -> NUM
    visit_factor_l_e_r, # Factor -> LPAREN Expression RPAREN
    visit_condition_statement, # ConditionStatement -> IF Condition THEN M Statement
    visit_m, # M -> EPSILON
    visit_loop_statement, # LoopStatement -> WHILE M Condition DO M Statement
    visit_condition, # Condition -> Expression RelationalOperator Expression
    visit_relop_eq,
    visit_relop_neq,
    visit_relop_lt,
    visit_relop_le,
    visit_relop_gt,
    visit_relop_ge,
    visit_empty_statement, # EmptyStatement -> EPSILON
    visit_identifier # Identifier -> ID
]

import re
def reduce_production(grammar, state_record, word_record, num):
    production = grammar[num]
    left = production.left
    reduce_length = len(production.right)
    if reduce_length == 1 and production.right[0].word == 'EPSILON':
        reduce_length = 0
    
    right = []
    for _ in range(reduce_length):
        state_record.pop()
        right.append(word_record.pop())
    
    new_top = state_record[-1]
    new_action = lr1_table[new_top].get(left)
    new_match = re.search(r'\d+', new_action)
    new_num = int(new_match.group())
    state_record.append(new_num)
    word_record.append(left)

    right.reverse()
    if visit_functions[num]:
        visit_functions[num](right, word_record)

    # print(f'reduce with {production} to state {state_record[-1]}')

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

def grammar_analyzer(word, state_record, word_record):
    global symbols, lr1_table
    # global state_record, word_record

    # prep = []

    # prep.extend(reversed(word_record))
    # word_record.clear()

    # for w in reversed(prep):
    #     word_record.append(w)

    top = state_record[-1]
    action = lr1_table[top].get(word)

    if not action:
        error('grammar error')
    
    match = re.search(r'\d+', action)
    num = None
    if match:
        num = int(match.group())

    return action, num

def show_mid_codes(mid_codes):
    for num, mid_code in mid_codes.items():
        print(f'{num}: {mid_code}')

def write_mid_codes_to_file(mid_codes, file_path):
    with open(file_path, 'w') as file:
        for num, mid_code in mid_codes.items():
            file.write(f'{num}: {mid_code}\n')

def read_src_code_from_file(file_path):
    with open(file_path, 'r') as file:
        file_content = file.read()
    return file_content

def compiler(src, file_path):
    read_from_pkl(file_path)
    index = 0
    state_record = [0]
    word_record = ['TERMINAL']
    need_next = True
    while True:
        try: 
            if need_next:
                lex, index = next(index)
            if lex:
                # print(f'current lex: {lex[1]}')
                action, num = grammar_analyzer(lex[1], state_record, word_record)
                # 移进
                if action.startswith('s'):
                    state_record.append(num)
                    word_record.append(lex)
                    # 跳到 next() 得到下一个 lex
                    need_next = True
                # 规约
                elif action.startswith('r'):
                    reduce_production(grammar, state_record, word_record, num)
                    # 保持原来的 lex
                    need_next = False
                # 接受
                elif action.startswith('acc'):
                    # print('finish successfully!')
                    break

        except ValueError as e:
            print(e)
            break
            
    show_mid_codes(mid_codes)
    write_mid_codes_to_file(mid_codes, './mid_codes.txt')

def test_grammer():
    words = ['PROGRAM', 'ID', 'CONST', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'VAR', 'ID', 'COMMA', 'ID', 'COMMA', 'ID', 'SEMI', 'BEGIN', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'WHILE', 'M', 'ID', 'LT', 'NUM', 'DO', 'M', 'ID', 'ASSIGN', 'ID', 'PLUS', 'ID', 'SEMI', 'IF', 'ID', 'NEQ', 'NUM', 'THEN', 'M', 'ID', 'ASSIGN', 'ID', 'MINUS', 'NUM', 'SEMI', 'IF', 'ID', 'LE', 'NUM', 'THEN', 'M', 'ID', 'ASSIGN', 'NUM', 'SEMI', 'IF', 'ID', 'GE', 'NUM', 'THEN', 'M', 'ID', 'ASSIGN', 'ID', 'PLUS', 'ID', 'MINUS', 'LPAREN', 'ID', 'TIMES', 'ID', 'DIV', 'ID', 'RPAREN', 'SEMI', 'ID', 'ASSIGN', 'ID', 'PLUS', 'ID', 'SEMI', 'END', 'TERMINAL']
    test_grammer_words(words)
        

if __name__ == '__main__':
    # index = 0
    # test()

    # read_lr1_table_from_csv()
    # csv_to_pkl('LR1.pkl')

    # test_grammer()

    # src = read_src_code_from_file('src_code.txt')
    compiler(src, 'LR1.pkl')

    # print(len(visit_functions))
    pass