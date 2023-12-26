from GraphList import GraphList
from Auto import Auto

class NFATrans:
    def __init__(self, start, end, recv):
        self.start = start
        self.end = end
        self.recv = recv

class NFAPartInfo:
    def __init__(self, start, end):
        self.start = start
        self.end = end

class NFAEndInfo:
    def __init__(self, regular="", priority=float('inf')):
        self.regular = regular
        self.priority = priority


class NFA(Auto):
    state_count = 0
    total_trans_list = []

    def __init__(self):
        self.start = 0
        self.end = 1
        self.end_info_map = {}
        super(NFA, self).__init__()

    def operate_create_new(self, new_content: str) -> NFAPartInfo:
        new_start = NFA.state_count
        new_end = NFA.state_count + 1
        NFA.state_count += 2

        self.start = new_start
        self.end = new_end
        self.start_list = {new_start}
        self.end_list = {new_end}

        self.trans_list.insert_vertex(new_start)
        self.trans_list.insert_vertex(new_end)

        new_start_pos = self.trans_list.get_vertex_pos(new_start)
        new_end_pos = self.trans_list.get_vertex_pos(new_end)

        if new_content == "l":
            self.trans_list.insert_edge(new_start_pos, new_end_pos, "<letter>")
            NFA.total_trans_list.append(NFATrans(new_start, new_end, "<letter>"))
        elif new_content == "d":
            self.trans_list.insert_edge(new_start_pos, new_end_pos, "<digit>")
            NFA.total_trans_list.append(NFATrans(new_start, new_end, "<digit>"))
        else:
            self.trans_list.insert_edge(new_start_pos, new_end_pos, new_content)
            NFA.total_trans_list.append(NFATrans(new_start, new_end, new_content))

        return NFAPartInfo(new_start, new_end)

    def operate_or(self, part1: NFAPartInfo, part2: NFAPartInfo) -> NFAPartInfo:
        new_start = NFA.state_count
        new_end = NFA.state_count + 1
        NFA.state_count += 2

        self.start = new_start
        self.end = new_end

        self.trans_list.insert_vertex(new_start)
        self.trans_list.insert_vertex(new_end)

        new_start_pos = self.trans_list.get_vertex_pos(new_start)
        new_end_pos = self.trans_list.get_vertex_pos(new_end)

        p1s_pos = self.trans_list.get_vertex_pos(part1.start)
        p1e_pos = self.trans_list.get_vertex_pos(part1.end)
        p2s_pos = self.trans_list.get_vertex_pos(part2.start)
        p2e_pos = self.trans_list.get_vertex_pos(part2.end)

        self.trans_list.insert_edge(new_start_pos, p1s_pos, "<epsilon>")
        self.trans_list.insert_edge(new_start_pos, p2s_pos, "<epsilon>")
        self.trans_list.insert_edge(p1e_pos, new_end_pos, "<epsilon>")
        self.trans_list.insert_edge(p2e_pos, new_end_pos, "<epsilon>")
        NFA.total_trans_list.append(NFATrans(new_start, part1.start, "<epsilon>"))
        NFA.total_trans_list.append(NFATrans(new_start, part2.start, "<epsilon>"))
        NFA.total_trans_list.append(NFATrans(part1.end, new_end, "<epsilon>"))
        NFA.total_trans_list.append(NFATrans(part2.end, new_end, "<epsilon>"))

        self.start_list = {new_start}
        self.end_list = {new_end}

        return NFAPartInfo(new_start, new_end)

    def operate_and(self, part1: NFAPartInfo, part2: NFAPartInfo) -> NFAPartInfo:
        p1e_pos = self.trans_list.get_vertex_pos(part1.end)
        p2s_pos = self.trans_list.get_vertex_pos(part2.start)

        self.trans_list.insert_edge(p1e_pos, p2s_pos, "<epsilon>")
        NFA.total_trans_list.append(NFATrans(part1.end, part2.start, "<epsilon>"))

        self.start = part1.start
        self.end = part2.end

        self.start_list.discard(part2.start)
        self.end_list.discard(part1.end)

        return NFAPartInfo(part1.start, part2.end)

    def operate_closure(self, part: NFAPartInfo) -> NFAPartInfo:
        new_start = NFA.state_count
        new_end = NFA.state_count + 1
        NFA.state_count += 2

        self.start = new_start
        self.end = new_end

        self.trans_list.insert_vertex(new_start)
        self.trans_list.insert_vertex(new_end)

        new_start_pos = self.trans_list.get_vertex_pos(new_start)
        new_end_pos = self.trans_list.get_vertex_pos(new_end)

        ps_pos = self.trans_list.get_vertex_pos(part.start)
        pe_pos = self.trans_list.get_vertex_pos(part.end)

        self.trans_list.insert_edge(new_start_pos, ps_pos, "<epsilon>")
        self.trans_list.insert_edge(pe_pos, ps_pos, "<epsilon>")
        self.trans_list.insert_edge(pe_pos, new_end_pos, "<epsilon>")
        self.trans_list.insert_edge(new_start_pos, new_end_pos, "<epsilon>")
        NFA.total_trans_list.append(NFATrans(new_start, part.start, "<epsilon>"))
        NFA.total_trans_list.append(NFATrans(part.end, part.start, "<epsilon>"))
        NFA.total_trans_list.append(NFATrans(part.end, new_end, "<epsilon>"))
        NFA.total_trans_list.append(NFATrans(new_start, new_end, "<epsilon>"))

        self.start_list = {new_start}
        self.end_list = {new_end}

        return NFAPartInfo(new_start, new_end)

    # 返回始终态信息
    def get_info(self) -> NFAPartInfo:
        return NFAPartInfo(self.start, self.end)

    def make_total_nfa(self, new_start: int, new_end_list, new_end_info_map):
        self.start = new_start
        self.start_list = {new_start}
        self.end_list = new_end_list

        for rule in NFA.total_trans_list:
            if self.trans_list.get_vertex_pos(rule.start) == -1:
                self.trans_list.insert_vertex(rule.start)
            if self.trans_list.get_vertex_pos(rule.end) == -1:
                self.trans_list.insert_vertex(rule.end)

            self.trans_list.insert_edge(
                self.trans_list.get_vertex_pos(rule.start),
                self.trans_list.get_vertex_pos(rule.end),
                rule.recv
            )

        self.end_info_map.clear()
        self.end_info_map.update(new_end_info_map)

    def show(self):
        print("-----------------------NFA-----------------------")
        print("start:", self.start)
        print("end:", self.end)
        print("NFA::state_count:", NFA.state_count)

        print("start_list:", self.start_list)
        print("end_list:", self.end_list)

        print("trans_list:")
        print(self.trans_list)

        print("end_info_map:")
        for state, info in self.end_info_map.items():
            print("状态:", state)
            print("正规式:", info.regular)
            print("优先级:", info.priority)


def test():
    nfa_instance = NFA()
    part1 = nfa_instance.operate_create_new("l")
    part2 = nfa_instance.operate_create_new("d")
    result = nfa_instance.operate_or(part1, part2)
    nfa_instance.show()

if __name__ == '__main__':
    test()
