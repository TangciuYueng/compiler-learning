# 边节点
class Edge:
    def __init__(self, dest, cost):
        # 节点值
        self.dest = dest
        self.cost = cost

    def __ne__(self, other):
        return self.dest != other.dest if isinstance(other, Edge) else NotImplemented


class Vertex:
    def __init__(self, data):
        # 节点对应的值
        self.data = data
        # 邻接表 edge 的 list
        self.adj = [] 


class GraphList:
    def __init__(self, if_directed):
        # 其中的元素为 Vertex
        self.node_table = []
        self.if_directed = if_directed
        self.data_to_index = {}

    def show_list(self):
        for vertex in self.node_table:
            for edge in vertex.adj:
                print(f"Vertex: {vertex.data}, Dest: {edge.dest}, Cost: {edge.cost}")

    # 传入参数为节点的值
    def get_vertex_pos(self, data):
        return self.data_to_index.get(data, -1)

    # 返回的是结点的值
    def get_vertex(self, pos):
        return self.node_table[pos].data if 0 <= pos < len(self.node_table) else None

    # 传入参数为节点的值 返回 cost
    def get_edge(self, v1, v2):
        index1 = self.get_vertex_pos(v1)
        if index1 == -1:
            return None
        index2 = self.get_vertex_pos(v2)
        if index2 == -1:
            return None

        vertex1 = self.node_table[index1]
        vertex2 = self.node_table[index2]

        if vertex1 and vertex2:
            for edge in vertex1.adj:
                if edge.dest == vertex2.data:
                    return edge.cost

        return None

    def get_neighbor_edges(self, data):
        pos = self.get_vertex_pos(data)
        if pos == -1:
            return []
        return self.node_table[pos].adj

    def insert_vertex(self, data):
        if self.get_vertex_pos(data) == -1:
            vertex = Vertex(data)
            self.node_table.append(vertex)
            # 加入映射表
            self.data_to_index[data] = len(self.node_table) - 1
            return True
        return False

    def insert_edge(self, v1, v2, cost):
        index1 = self.get_vertex_pos(v1)
        if index1 == -1:
            return None
        index2 = self.get_vertex_pos(v2)
        if index2 == -1:
            return None

        vertex1 = self.node_table[index1]
        vertex2 = self.node_table[index2]

        if vertex1 and vertex2:
            new_edge = Edge(vertex2.data, cost)
            vertex1.adj.append(new_edge)

            if not self.if_directed:
                new_edge = Edge(vertex1.data, cost)
                vertex2.adj.append(new_edge)
            
            return True

        return False

    # 传入节点值
    def remove_vertex(self, v):
        # 获取这个
        pos = self.get_vertex_pos(v)
        if pos != -1:
            # 删除别的节点和待删除节点的边
            for vertex in self.node_table:
                vertex.adj = [edge for edge in vertex.adj if edge.dest != v]

            # 再删除找到的这个节点
            self.node_table.pop(pos)
            return True
        return False

    def remove_edge(self, v1, v2):
        index1 = self.get_vertex_pos(v1)
        if index1 == -1:
            return False
        index2 = self.get_vertex_pos(v2)
        if index2 == -1:
            return False

        vertex1 = self.node_table[index1]
        vertex2 = self.node_table[index2]

        if vertex1 and vertex2:
            # 删除v1到v2的边
            vertex1.adj = [edge for edge in vertex1.adj if edge.dest != v2]
            
            if not self.if_directed:
                # 如果是无向图，同时删除v2到v1的边
                vertex2.adj = [edge for edge in vertex2.adj if edge.dest != v1]

            return True

        return False

    def __str__(self):
        result = ""
        for vertex in self.node_table:
            for edge in vertex.adj:
                result += f'Vertex: {vertex.data}, Dest: {edge.dest}, Cost: {edge.cost}\n'

        return result


def test():
    graph_instance = GraphList(True)
    graph_instance.insert_vertex("A")
    graph_instance.insert_vertex("B")
    graph_instance.insert_vertex("C")
    graph_instance.insert_edge("A", "B", 1)
    graph_instance.insert_edge("B", "C", 2)
    print(graph_instance)

if __name__ == "__main__":
    test()
