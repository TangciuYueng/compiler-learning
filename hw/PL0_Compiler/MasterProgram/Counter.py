class Counter:
    def __init__(self, initial_value=0):
        self.value = initial_value

    def increment(self):
        current_value = self.value
        self.value += 1
        return current_value


class TempVarCounter(Counter):
    def new_temp_var(self):
        return "t" + str(self.increment())


class MidCodeCounter(Counter):
    def get_mid_code_counter(self):
        return self.increment()

    def get_current_mid_code_counter(self):
        return self.value


class ConditionCounter(Counter):
    def get_condition_counter(self):
        return self.increment()


class NextCounter(Counter):
    def get_next_counter(self):
        return self.increment()


def test():
    temp_var_counter = TempVarCounter()
    mid_code_counter = MidCodeCounter(100)
    condition_counter = ConditionCounter()
    next_counter = NextCounter()

    print(temp_var_counter.new_temp_var())  # 输出: t0
    print(mid_code_counter.get_mid_code_counter())  # 输出: 0
    print(condition_counter.get_condition_counter())  # 输出: 0
    print(next_counter.get_next_counter())  # 输出: 0
    print(mid_code_counter.get_current_mid_code_counter())  # 输出: 1


if __name__ == '__main__':
    test()
