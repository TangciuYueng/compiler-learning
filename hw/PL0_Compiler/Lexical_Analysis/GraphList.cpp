#include"GraphList.h"

namespace toolkit {
	//重载函数：输出
	template<class _vertex, class _edge>
	ostream& operator<<(ostream& out, const GraphList<_vertex, _edge>& Graph)
	{
		const int ver_num = Graph.NumOfVertices();
		const int edge_num = Graph.NumOfEdges();
		out << "vertices:" << ver_num << endl;
		out << "edges:" << edge_num << endl;

		for (int i = 0; i < ver_num; i++) {
			for (int j = 0; j < ver_num; j++) {  //无向图从i+1开始
				_edge e;
				bool if_get = Graph.getEdge(e, i, j);

				if (if_get) {
					_vertex v1, v2;
					Graph.getVertex(v1, i);
					Graph.getVertex(v2, j);
					out << '<' << v1 << ',' << v2 << '>' << "-->" << e << endl;
				}
			}
		}

		return out;
	}

	//指定大小的构造函数
	template<class _vertex, class _edge>
	GraphList<_vertex, _edge>::GraphList(const bool ifDirected, const int sz) : if_directed(ifDirected)
	{
		maxVertices = sz;
		numVertices = 0;
		numEdges = 0;

		NodeTable = new Vertex<_vertex, _edge>[maxVertices];
		if (NodeTable == NULL) {
			cerr << "存储分配错误！" << endl;
			exit(1);
		}

		for (int i = 0; i < maxVertices; i++)
			NodeTable[i].adj = NULL;
	}

	//复制构造函数
	template<class _vertex, class _edge>
	GraphList<_vertex, _edge>::GraphList(const bool ifDirected, const GraphList<_vertex, _edge>& Graph) : if_directed(ifDirected)
	{
		maxVertices = Graph.maxVertices;
		numVertices = Graph.numVertices;
		numEdges = Graph.numEdges;

		NodeTable = new Vertex<_vertex, _edge>[maxVertices];
		if (NodeTable == NULL) {
			cerr << "存储分配失败！" << endl;
			exit(1);
		}

		for (int i = 0; i < numVertices; i++) {
			NodeTable[i].data = Graph.NodeTable[i].data;

			Edge<_vertex, _edge>* src = Graph.NodeTable[i].adj, * newNode, * current = NULL;

			if (src == NULL)
				NodeTable[i].adj = NULL;

			while (src != NULL) {
				newNode = new Edge<_vertex, _edge>(src->dest, src->cost);
				if (newNode == NULL) {
					cerr << "存储分配失败！" << endl;
					exit(1);
				}

				if (src == Graph.NodeTable[i].adj)
					NodeTable[i].adj = newNode;
				else
					current->link = newNode;
				current = newNode;

				src = src->link;
			}
		}
	}

	//析构函数
	template<class _vertex, class _edge>
	GraphList<_vertex, _edge>::~GraphList()
	{
		for (int i = 0; i < numVertices; i++) {
			Edge<_vertex, _edge>* p = NodeTable[i].adj;

			while (p != NULL) {
				NodeTable[i].adj = p->link;
				delete p;
				p = NodeTable[i].adj;
			}
		}

		delete[] NodeTable;
	}

	//返回顶点vertex在顶点表中的位置
	template<class _vertex, class _edge>
	int GraphList<_vertex, _edge>::getVertexPos(_vertex vertex)const
	{
		for (int i = 0; i < numVertices; i++) {
			if (NodeTable[i].data == vertex)
				return i;
		}

		return -1;
	}

	//返回当前顶点数
	template<class _vertex, class _edge>
	int GraphList<_vertex, _edge>::NumOfVertices()const
	{
		return numVertices;
	}

	//返回当前边数
	template<class _vertex, class _edge>
	int GraphList<_vertex, _edge>::NumOfEdges()const
	{
		return numEdges;
	}

	//展示邻接表
	template<class _vertex, class _edge>
	void GraphList<_vertex, _edge>::ShowList()const
	{
		for (int i = 0; i < numVertices; i++) {
			cout << NodeTable[i].data << "->";
			Edge<_vertex, _edge>* p = NodeTable[i].adj;

			while (p != NULL) {
				if (p->link != NULL)
					cout << p->dest << "->";
				else
					cout << p->dest << endl;

				p = p->link;
			}
		}
	}

	//判断图的顶点数是否为零
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::IsVerticesEmpty()const
	{
		return (numVertices == 0);
	}

	//判断图的边数是否为零
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::IsEdgesEmpty()const
	{
		return (numEdges == 0);
	}

	//判断图的顶点数是否已满
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::IsVerticesFull()const
	{
		if (numVertices == maxVertices)
			return true;
		else
			return false;
	}

	//判断图的边数是否已满
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::IsEdgesFull()const
	{
		const int limit = if_directed ? numVertices * (numVertices - 1) : numVertices * (numVertices - 1) / 2;

		if (numEdges == limit)
			return true;
		else
			return false;
	}

	//取顶点pos，pos不合理返回false
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::getVertex(_vertex& vertex, const int pos)const
	{
		if (pos < 0 || pos >= numVertices)
			return false;

		vertex = NodeTable[pos].data;
		return true;
	}

	//取边<v1,v2>或(v1,v2)上的权值
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::getEdge(_edge& edge, const int v1, const int v2)const
	{
		if (v1 < 0 || v1 >= numVertices || v2 < 0 || v2 >= numVertices)
			return false;

		Edge<_vertex, _edge>* p = NodeTable[v1].adj;

		while (p != NULL && p->dest != v2)
			p = p->link;

		if (p != NULL) {
			edge = p->cost;
			return true;
		}

		return false;
	}

	//取顶点v的第一个邻接顶点
	template<class _vertex, class _edge>
	int GraphList<_vertex, _edge>::getFirstNeighbor(const int v)const
	{
		if (v >= 0 && v < numVertices) {
			Edge<_vertex, _edge>* p = NodeTable[v].adj;

			if (p != NULL)
				return p->dest;
		}

		return -1;
	}

	//取v邻接顶点w的下一邻接顶点
	template<class _vertex, class _edge>
	int GraphList<_vertex, _edge>::getNextNeighbor(const int v, const int w)const
	{
		if (v >= 0 && v < numVertices && w >= 0 && w < numVertices) {
			Edge<_vertex, _edge>* p = NodeTable[v].adj;

			while (p != NULL && p->dest != w)
				p = p->link;

			if (p != NULL && p->link != NULL)
				return p->link->dest;
		}

		return -1;
	}

	//插入顶点vertex
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::insertVertex(const _vertex vertex)
	{
		if (numVertices == maxVertices)
			return false;

		NodeTable[numVertices++].data = vertex;
		return true;
	}

	//插入边<v1,v2>，权值为cost
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::insertEdge(const int v1, const int v2, _edge cost)
	{
		if (v1 >= 0 && v1 < numVertices && v2 >= 0 && v2 < numVertices) {
			Edge<_vertex, _edge>* p = NodeTable[v1].adj;

			while (p != NULL && p->dest != v2)
				p = p->link;

			if (p != NULL)
				return false;

			p = new Edge<_vertex, _edge>(v2, cost, NodeTable[v1].adj);
			if (p == NULL) {
				cerr << "存储分配失败！" << endl;
				exit(1);
			}
			NodeTable[v1].adj = p;

			numEdges++;

			if (if_directed)
				return true;
		}

		if (!if_directed && v1 >= 0 && v1 < numVertices && v2 >= 0 && v2 < numVertices) {
			Edge<_vertex, _edge>* p = NodeTable[v2].adj;

			p = new Edge<_vertex, _edge>(v1, cost, NodeTable[v2].adj);
			if (p == NULL) {
				cerr << "存储分配失败！" << endl;
				exit(1);
			}
			NodeTable[v2].adj = p;

			return true;
		}

		return false;
	}

	//删去顶点v和所有与它相关联的边
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::removeVertex(const int v)
	{
		if (v < 0 || v >= numVertices)
			return false;

		if (numVertices == 1)
			return false;

		Edge<_vertex, _edge>* p, * s;

		/*1、删除该顶点的出边表*/
		while (NodeTable[v].adj != NULL) {
			p = NodeTable[v].adj;
			NodeTable[v].adj = p->link;
			delete p;
			numEdges--;
		}

		/*2、删除该顶点的入边*/
		for (int i = 0; i < numVertices; i++) {
			if (i == v)
				continue;

			p = NodeTable[i].adj;
			s = NULL;

			while (p != NULL && p->dest != v) {
				s = p;
				p = p->link;
			}

			if (p != NULL) {
				if (s == NULL)
					NodeTable[i].adj = p->link;
				else
					s->link = p->link;

				if (if_directed)
					numEdges--;

				delete p;
				continue;
			}
		}

		/*3、最后一个顶点顶上删除顶点的位置，并更换数据*/
		NodeTable[v] = NodeTable[numVertices - 1];
		for (int i = 0; i < numVertices; i++) {
			if (i == v)
				continue;

			else if (i == numVertices - 1) {
				NodeTable[i].adj = NULL;
				continue;
			}

			p = NodeTable[i].adj;

			while (p != NULL && p->dest != numVertices - 1)
				p = p->link;

			if (p != NULL) {
				p->dest = v;
				continue;
			}
		}

		numVertices--;
		return true;
	}

	//在图中删去边(v1,v2)或<v1,v2>
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::removeEdge(const int v1, const int v2)
	{
		if (v1 >= 0 && v1 < numVertices && v2 >= 0 && v2 < numVertices) {
			Edge<_vertex, _edge>* p = NodeTable[v1].adj, * q = NULL, * s = p;

			while (p != NULL && p->dest != v2) {
				q = p;
				p = p->link;
			}

			if (p != NULL) {
				if (p == s)
					NodeTable[v1].adj = p->link;
				else
					q->link = p->link;

				numEdges--;
				delete p;
			}

			else
				return false;

			if (if_directed)
				return true;

			p = NodeTable[v2].adj;
			q = NULL;
			s = p;

			while (p->dest != v1) {
				q = p;
				p = p->link;
			}

			if (p == s)
				NodeTable[v2].adj = p->link;
			else
				q->link = p->link;

			delete p;
			return true;
		}

		return false;
	}

	//重载函数：赋值
	template<class _vertex, class _edge>
	GraphList<_vertex, _edge>& GraphList<_vertex, _edge>::operator=(const GraphList<_vertex, _edge>& graph)
	{
		for (int i = 0; i < numVertices; i++) {
			Edge<_vertex, _edge>* p = NodeTable[i].adj;

			while (p != NULL) {
				NodeTable[i].adj = p->link;
				delete p;
				p = NodeTable[i].adj;
			}
		}

		delete[] NodeTable;

		maxVertices = graph.maxVertices;
		numVertices = graph.numVertices;
		numEdges = graph.numEdges;

		NodeTable = new Vertex<_vertex, _edge>[maxVertices];
		if (NodeTable == NULL) {
			cerr << "存储分配失败！" << endl;
			exit(1);
		}

		for (int i = 0; i < numVertices; i++) {
			NodeTable[i].data = graph.NodeTable[i].data;

			Edge<_vertex, _edge>* src = graph.NodeTable[i].adj, * newNode, * current = NULL;

			if (src == NULL)
				NodeTable[i].adj = NULL;

			while (src != NULL) {
				newNode = new Edge<_vertex, _edge>(src->dest, src->cost);
				if (newNode == NULL) {
					cerr << "存储分配失败！" << endl;
					exit(1);
				}

				if (src == graph.NodeTable[i].adj)
					NodeTable[i].adj = newNode;
				else
					current->link = newNode;
				current = newNode;

				src = src->link;
			}
		}

		return *this;
	}
}