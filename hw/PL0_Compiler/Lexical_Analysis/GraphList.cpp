#include"GraphList.h"

namespace toolkit {
	//���غ��������
	template<class _vertex, class _edge>
	ostream& operator<<(ostream& out, const GraphList<_vertex, _edge>& Graph)
	{
		const int ver_num = Graph.NumOfVertices();
		const int edge_num = Graph.NumOfEdges();
		out << "vertices:" << ver_num << endl;
		out << "edges:" << edge_num << endl;

		for (int i = 0; i < ver_num; i++) {
			for (int j = 0; j < ver_num; j++) {  //����ͼ��i+1��ʼ
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

	//ָ����С�Ĺ��캯��
	template<class _vertex, class _edge>
	GraphList<_vertex, _edge>::GraphList(const bool ifDirected, const int sz) : if_directed(ifDirected)
	{
		maxVertices = sz;
		numVertices = 0;
		numEdges = 0;

		NodeTable = new Vertex<_vertex, _edge>[maxVertices];
		if (NodeTable == NULL) {
			cerr << "�洢�������" << endl;
			exit(1);
		}

		for (int i = 0; i < maxVertices; i++)
			NodeTable[i].adj = NULL;
	}

	//���ƹ��캯��
	template<class _vertex, class _edge>
	GraphList<_vertex, _edge>::GraphList(const bool ifDirected, const GraphList<_vertex, _edge>& Graph) : if_directed(ifDirected)
	{
		maxVertices = Graph.maxVertices;
		numVertices = Graph.numVertices;
		numEdges = Graph.numEdges;

		NodeTable = new Vertex<_vertex, _edge>[maxVertices];
		if (NodeTable == NULL) {
			cerr << "�洢����ʧ�ܣ�" << endl;
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
					cerr << "�洢����ʧ�ܣ�" << endl;
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

	//��������
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

	//���ض���vertex�ڶ�����е�λ��
	template<class _vertex, class _edge>
	int GraphList<_vertex, _edge>::getVertexPos(_vertex vertex)const
	{
		for (int i = 0; i < numVertices; i++) {
			if (NodeTable[i].data == vertex)
				return i;
		}

		return -1;
	}

	//���ص�ǰ������
	template<class _vertex, class _edge>
	int GraphList<_vertex, _edge>::NumOfVertices()const
	{
		return numVertices;
	}

	//���ص�ǰ����
	template<class _vertex, class _edge>
	int GraphList<_vertex, _edge>::NumOfEdges()const
	{
		return numEdges;
	}

	//չʾ�ڽӱ�
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

	//�ж�ͼ�Ķ������Ƿ�Ϊ��
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::IsVerticesEmpty()const
	{
		return (numVertices == 0);
	}

	//�ж�ͼ�ı����Ƿ�Ϊ��
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::IsEdgesEmpty()const
	{
		return (numEdges == 0);
	}

	//�ж�ͼ�Ķ������Ƿ�����
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::IsVerticesFull()const
	{
		if (numVertices == maxVertices)
			return true;
		else
			return false;
	}

	//�ж�ͼ�ı����Ƿ�����
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::IsEdgesFull()const
	{
		const int limit = if_directed ? numVertices * (numVertices - 1) : numVertices * (numVertices - 1) / 2;

		if (numEdges == limit)
			return true;
		else
			return false;
	}

	//ȡ����pos��pos��������false
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::getVertex(_vertex& vertex, const int pos)const
	{
		if (pos < 0 || pos >= numVertices)
			return false;

		vertex = NodeTable[pos].data;
		return true;
	}

	//ȡ��<v1,v2>��(v1,v2)�ϵ�Ȩֵ
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

	//ȡ����v�ĵ�һ���ڽӶ���
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

	//ȡv�ڽӶ���w����һ�ڽӶ���
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

	//���붥��vertex
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::insertVertex(const _vertex vertex)
	{
		if (numVertices == maxVertices)
			return false;

		NodeTable[numVertices++].data = vertex;
		return true;
	}

	//�����<v1,v2>��ȨֵΪcost
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
				cerr << "�洢����ʧ�ܣ�" << endl;
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
				cerr << "�洢����ʧ�ܣ�" << endl;
				exit(1);
			}
			NodeTable[v2].adj = p;

			return true;
		}

		return false;
	}

	//ɾȥ����v����������������ı�
	template<class _vertex, class _edge>
	bool GraphList<_vertex, _edge>::removeVertex(const int v)
	{
		if (v < 0 || v >= numVertices)
			return false;

		if (numVertices == 1)
			return false;

		Edge<_vertex, _edge>* p, * s;

		/*1��ɾ���ö���ĳ��߱�*/
		while (NodeTable[v].adj != NULL) {
			p = NodeTable[v].adj;
			NodeTable[v].adj = p->link;
			delete p;
			numEdges--;
		}

		/*2��ɾ���ö�������*/
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

		/*3�����һ�����㶥��ɾ�������λ�ã�����������*/
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

	//��ͼ��ɾȥ��(v1,v2)��<v1,v2>
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

	//���غ�������ֵ
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
			cerr << "�洢����ʧ�ܣ�" << endl;
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
					cerr << "�洢����ʧ�ܣ�" << endl;
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