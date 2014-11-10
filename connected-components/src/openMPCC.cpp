#include <vector>
#include <omp.h>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/connected_components.hpp>
#include <iostream>

using namespace std;
using namespace boost;


typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;

int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
	int numThreads = omp_get_num_threads();
	int componentCount=0;
	cout << "run MPCC";

	#pragma omp parallel reduction(+:componentCount)
	{
		int tid = omp_get_thread_num();
		int rangeSize = edges.size()/numThreads;
		int rangeStart = tid*rangeSize;
		int rangeEnd = rangeStart + rangeSize;

		Graph g(numberOfVertices);
		for (unsigned int i = rangeStart; i < rangeSize; ++i) {
			add_edge(edges[i].first, edges[i].second,g);
		}
		componentCount = connected_components(g, &outVertexToComponent[0]);
	}


	return componentCount;
}


