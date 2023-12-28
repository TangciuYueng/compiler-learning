class Element:
    def __init__(self, word, if_terminal):
        self.word = word
        self.if_terminal = if_terminal

    def __str__(self):
        return f'word({self.word})'

    def __repr__(self):
        return self.__str__()

    def __hash__(self):
        return hash(self.word)

    def __eq__(self, other):
        return self.word == other.word and self.if_terminal == other.if_terminal

class Production:
    def __init__(self, left, right):
        self.left = left
        if not right:
            self.right = [Element("EPSILON", True)]
        else:
            self.right = right

    def __str__(self):
        return f"{self.left} -> {self.right}"

    def __repr__(self):
        return self.__str__()

    def __hash__(self):
        return hash((self.left, tuple(hash(element) for element in self.right)))

    def __eq__(self, other):
        return self.left == other.left and self.right == other.right


class DFAProduction:
    def __init__(self, left: str, right, outlook, point=0):
        self.left = left
        self.right = [element for element in right if element.word != "EPSILON"]
        self.point = point
        self.outlook = outlook
    
    def __hash__(self):
        return hash((self.left, tuple((element.word, element.if_terminal) for element in self.right),
                     self.point, tuple(sorted(self.outlook))))

    def __eq__(self, other):
        if not isinstance(other, DFAProduction):
            return False
        return (
            self.left == other.left
            and len(self.right) == len(other.right)
            and all(x == y for x, y in zip(self.right, other.right))
            and self.point == other.point
            and sorted(self.outlook) == sorted(other.outlook)
        )

    def __str__(self):
        result = f"{self.left} -> "
        for i, element in enumerate(self.right):
            if i == self.point:
                result += "·"
            result += f"{element.word} "
        if self.point == len(self.right):
            result += "·"
        result += "(" + ", ".join(self.outlook) + ")"
        return result

    def __repr__(self):
        return self.__str__()

    def begin(self) -> bool:
        return self.point == 0

    def end(self) -> bool:
        return self.point == len(self.right)

    def next_end(self) -> bool:
        return self.end() or self.point + 1 == len(self.right)

    def add(self) -> None:
        if not self.end():
            self.point += 1

    def get_point(self) -> Element:
        if not self.end():
            return self.right[self.point]
        return Element("ERROR", True)

def test():
    # 创建一些示例 DFAProduction 实例
    dfa_production1 = DFAProduction("A", [Element("a", True), Element("b", False)], {"outlook1", "outlook2"})
    dfa_production2 = DFAProduction("B", [Element("b", True)], {"outlook3", "outlook4"})

    list1 = []
    list1.append(DFAProduction(dfa_production1.left, dfa_production1.right, dfa_production1.outlook, dfa_production1.point))
    list1.append(dfa_production2)

    list2 = []
    list2.append(dfa_production2)
    list2.append(dfa_production1)

    print(set(list1) == set(list2))

if __name__ == '__main__':
    test()

