class RCOperand:
    def __init__(self, value, info):
        self.value = value
        self.info = info

class RegularCalculator:
    postfixStack = []
    operateStack = []

    @staticmethod
    def ISP(ch):
        if ch == '#':
            return 0
        elif ch == '(':
            return 1
        elif ch == '`':
            return 7
        elif ch == '$':
            return 5
        elif ch == '|':
            return 3
        elif ch == ')':
            return 8
        else:
            return -1

    @staticmethod
    def ICP(ch):
        if ch == '#':
            return 0
        elif ch == '(':
            return 8
        elif ch == '`':
            return 6
        elif ch == '$':
            return 4
        elif ch == '|':
            return 2
        elif ch == ')':
            return 1
        else:
            return -1

    @staticmethod
    def IsOp(ch):
        return ch in ['|', '$', '`']

    @staticmethod
    def PostfixPrep(exp):
        if not exp:
            return ""

        result = []
        index = 0
        while index < len(exp):
            result.append(exp[index])

            if (exp[index] != '(') and (index + 1 < len(exp)) and (not RegularCalculator.IsOp(exp[index + 1])) and (exp[index + 1] != ')'):
                if exp[index + 1] == '(' and exp[index] != '(':
                    result.append('$')
                elif exp[index] != '|' and exp[index] != '$':
                    result.append('$')

            index += 1
        # result.append('#')
        return ''.join(result)

    @staticmethod
    def Postfix(src):
        result = []
        iter_index = 0

        def get_next_char():
            nonlocal iter_index
            if iter_index < len(src):
                char = src[iter_index]
                iter_index += 1
                return char
            return None

        ch = '#'
        RegularCalculator.postfixStack.append(ch)
        ch = get_next_char()

        while RegularCalculator.postfixStack:
            if ch is not None and not RegularCalculator.IsOp(ch) and ch != '(' and ch != ')':
                result.append(ch)
                ch = get_next_char()
            else:
                top = RegularCalculator.postfixStack[-1]

                if RegularCalculator.ISP(top) < RegularCalculator.ICP(ch):
                    RegularCalculator.postfixStack.append(ch)
                    ch = get_next_char()
                elif RegularCalculator.ISP(top) > RegularCalculator.ICP(ch):
                    pop = RegularCalculator.postfixStack.pop()
                    result.append(pop)
                else:
                    pop = RegularCalculator.postfixStack.pop()
                    if pop == '(':
                        ch = get_next_char()

        return ''.join(result)

    @staticmethod
    def AddOperand(value, nfa):
        NFAPartInfo = nfa.operate_create_new(value)
        RegularCalculator.operateStack.append(RCOperand(value, NFAPartInfo))

    @staticmethod
    def GetOperand():
        if not RegularCalculator.operateStack:
            print("栈已空！")
            return None
        return RegularCalculator.operateStack.pop()

    @staticmethod
    def Get2Operand():
        if not RegularCalculator.operateStack:
            print("缺少右操作数！")
            return None, None
        right = RegularCalculator.operateStack.pop()

        if not RegularCalculator.operateStack:
            print("缺少左操作数！")
            return None, None
        left = RegularCalculator.operateStack.pop()

        return left, right

    @staticmethod
    def DoOperation(op, nfa):
        left, right = None, None
        flag = False

        if op == '`':
            left = RegularCalculator.GetOperand()
            if left:
                result_info = nfa.operate_closure(left.info)
                result_value = '(' + left.value + ')`'
                RegularCalculator.operateStack.append(RCOperand(result_value, result_info))
                return

        else:
            left, right = RegularCalculator.Get2Operand()
            flag = True

        if flag:
            if op == '|':
                result_value = left.value + '|' + right.value
                result_info = nfa.operate_or(left.info, right.info)
                RegularCalculator.operateStack.append(RCOperand(result_value, result_info))
            elif op == '$':
                result_value = left.value + right.value
                result_info = nfa.operate_and(left.info, right.info)
                RegularCalculator.operateStack.append(RCOperand(result_value, result_info))

    @staticmethod
    def Calculation(exp, nfa):
        iter_index = 0
        ch = exp[iter_index]

        while ch != '#':
            if ch in ['|', '$', '`']:
                RegularCalculator.DoOperation(ch, nfa)
            else:
                operand = ch
                RegularCalculator.AddOperand(operand, nfa)

            iter_index += 1
            ch = exp[iter_index]

