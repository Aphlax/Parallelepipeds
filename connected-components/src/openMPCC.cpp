#include <vector>
#include <omp.h>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/connected_components.hpp>

using namespace std;
using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;

int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
	Graph g(numberOfVertices);
	for (unsigned int i = 0; i < edges.size(); ++i) {
		add_edge(edges[i].first, edges[i].second,g);
	}

	int componentCount = connected_components(g, &outVertexToComponent[0]);
	return componentCount;
}

