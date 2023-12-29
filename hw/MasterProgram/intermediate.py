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


def reset():
    global temp_var_counter, mid_code_counter, condition_counter, next_counter, mid_codes, true_lists, false_lists,\
        next_lists, existing_symbols

    temp_var_counter = TempVarCounter()
    mid_code_counter = MidCodeCounter(100)
    condition_counter = ConditionCounter()
    next_counter = NextCounter()
    mid_codes = {}
    true_lists = {}
    false_lists = {}
    next_lists = {}
    existing_symbols = {}


def get_mid_codes():
    ret = ""

    for num, mid_code in mid_codes.items():
        ret += f'{num}: {mid_code}\n'

    return ret


def error(message):
    raise ValueError(f"Intermediate error: {message}")


def visit_constant_definition(node, word_record):
    # ConstantDefinition -> ID ASSIGN NUM
    if node[0] in existing_symbols:
        error(f'{node[0][0]} has been declared')
    else:
        existing_symbols[node[0][0]] = "CONST"
    cnt = mid_code_counter.get_mid_code_counter()
    mid_codes[cnt] = f'{node[0][0]} := {node[2][0]}'


def visit_variable_declaration(node, word_record):
    # VariableDeclaration -> VAR ID NextID
    if node[1] in existing_symbols:
        error(f'{node[1][0]} has been declared')
    else:
        existing_symbols[node[1][0]] = "VAR"


def visit_next_id(node, word_record):
    # NextID -> COMMA ID NextID
    if node[1][0] in existing_symbols:
        error(f'{node[1][0]} has been declared')
    else:
        existing_symbols[node[1][0]] = "VAR"


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
    if node[0] not in existing_symbols:
        error(f'{node[0]} has not been declared!')

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
    if node[0] not in existing_symbols:
        error(f'{node[0]} has not been declared!')

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
    None,  # Program` -> Program
    None,  # Program -> PROGRAM ID SubProgram
    None,  # SubProgram -> ConstantDeclaration VariableDeclaration Statement
    None,  # SubProgram -> ConstantDeclaration Statement
    None,  # SubProgram -> VariableDeclaration Statement
    None,  # SubProgram -> Statement
    None,  # ConstantDeclaration -> CONST ConstantDefinition NextConstantDefinition
    None,  # NextConstantDefinition -> COMMA ConstantDefinition NextConstantDefinition
    None,  # NextConstantDefinition -> SEMI
    visit_constant_definition,  # ConstantDefinition -> ID ASSIGN NUM
    visit_variable_declaration,  # VariableDeclaration -> VAR ID NextID
    visit_next_id,  # NextID -> COMMA ID NextID
    None,  # NextID -> SEMI
    visit_compound_statement,  # CompoundStatement -> BEGIN LastStatement Statement END
    visit_last_statement,  # LastStatement -> LastStatement SEMI M Statement
    visit_last_statement_single,  # LastStatement -> Statement
    visit_statement_assignment,  # Statement -> AssignmentStatement
    visit_statement_condition,  # Statement -> ConditionStatement
    visit_statement_loop,  # Statement -> LoopStatement
    visit_statement_compound,  # Statement -> CompoundStatement
    visit_statement_empty,  # Statement -> EmptyStatement
    visit_assignment_statement,  # AssignmentStatement -> Identifier ASSIGN Expression
    visit_expression_plus_term,  # Expression -> PLUS Term
    visit_expression_minus_term,  # Expression -> MINUS Term
    visit_expression_term,  # Expression -> Term
    visit_expression_e_p_t,  # Expression -> Expression PLUS Term
    visit_expression_e_m_t,  # Expression -> Expression MINUS Term
    visit_term_factor,  # Term -> Factor
    visit_term_t_t_f,  # Term -> Term TIMES Factor
    visit_term_t_d_f,  # Term -> Term DIV Factor
    visit_factor_i,  # Factor -> Identifier
    visit_factor_n,  # Factor -> NUM
    visit_factor_l_e_r,  # Factor -> LPAREN Expression RPAREN
    visit_condition_statement,  # ConditionStatement -> IF Condition THEN M Statement
    visit_m,  # M -> EPSILON
    visit_loop_statement,  # LoopStatement -> WHILE M Condition DO M Statement
    visit_condition,  # Condition -> Expression RelationalOperator Expression
    visit_relop_eq,  # RelationalOperator -> EQ
    visit_relop_neq,  # RelationalOperator -> NEQ
    visit_relop_lt,  # RelationalOperator -> LT
    visit_relop_le,  # RelationalOperator -> LE
    visit_relop_gt,  # RelationalOperator -> GT
    visit_relop_ge,  # RelationalOperator -> GE
    visit_empty_statement,  # EmptyStatement -> EPSILON
    visit_identifier  # Identifier -> ID
]