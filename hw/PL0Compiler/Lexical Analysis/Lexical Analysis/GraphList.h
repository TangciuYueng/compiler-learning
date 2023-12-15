#ifndef _GRAPHLIST_H_
#define _GRAPHLIST_H_

#include<iostream>
#include<stdlib.h>
#include<climits>
#include"D:/Tool Kit/LinkedQueue/LinkedQueue.h"
using namespace std;

namespace toolkit {
	const int LIST_VERTICES_DEFAULT_SIZE = 20;

	template<class _vertex, class _edge>
	struct Edge {
		int dest;
		_edge cost;
		Edge<_vertex, _edge>* link;

		Edge() :dest(-1), link(NULL) {}
		Edge(const int num, const _edge weight, Edge<_vertex, _edge>* first = NULL) :dest(num), cost(weight), link(first) {}

		inline bool operator!=(const Edge<_vertex, _edge>& edge)const
		{
			return (dest != edge.dest);
		}
	};

	template<class _vertex, class _edge>
	struct Vertex {
		_vertex data;
		Edge<_vertex, _edge>* adj;
	};

	template<class _vertex, class _edge>
	class GraphList {
	private:
		int maxVertices;
		int numEdges;
		int numVertices;
		Vertex<_vertex, _edge>* NodeTable;
		const bool if_directed;

	public:
		GraphList(const bool ifDirected, const int sz = LIST_VERTICES_DEFAULT_SIZE);
		GraphList(const bool ifDirected, const GraphList<_vertex, _edge>& graph);
		~GraphList();

		int NumOfVertices()const;
		int NumOfEdges()const;
		void ShowList()const;
		bool IsVerticesEmpty()const;
		bool IsEdgesEmpty()const;
		bool IsVerticesFull()const;
		bool IsEdgesFull()const;
		int getVertexPos(_vertex vertex)const;
		bool getVertex(_vertex& vertex, const int pos)const;
		bool getEdge(_edge& edge, const int v1, const int v2)const;
		int getFirstNeighbor(const int v)const;
		int getNextNeighbor(const int v, const int w)const;
		bool insertVertex(const _vertex vertex);
		bool insertEdge(const int v1, const int v2, _edge cost);
		bool removeVertex(const int v);
		bool removeEdge(const int v1, const int v2);

		GraphList<_vertex, _edge>& operator=(const GraphList<_vertex, _edge>& graph);

		template<class V, class E>  //不是成员函数，需要有自己的模板
		friend ostream& operator<<(ostream& out, const GraphList<V, E>& Graph);
	};
}

#endif
