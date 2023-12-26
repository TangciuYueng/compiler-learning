from GraphList import GraphList

class Auto:
    def __init__(self):
        self.start_list = set()  # 起始态列表
        self.end_list = set()    # 终止态列表
        self.trans_list = GraphList(True)  # 状态转换图
