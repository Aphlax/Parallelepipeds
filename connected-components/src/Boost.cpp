#include <vector>
#include <omp.h>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/connected_components.hpp>
#include <chrono>
#include "StopWatch.cpp"

using namespace std;
using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> GraphS;
typedef graph_traits<GraphS>::vertex_descriptor VertexS;

class Boost {

public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
		StopWatch stopWatch;
		stopWatch.start(stopWatch.inputProcessing);

		GraphS g(numberOfVertices);
		for (unsigned int i = 0; i < edges.size(); ++i) {
			add_edge(edges[i].first, edges[i].second,g);
		}

		cout << "Checkpoint 1: " << stopWatch.stop(stopWatch.inputProcessing) << "s\n";
		stopWatch.start(stopWatch.mainSection);
		int componentCount = connected_components(g, &outVertexToComponent[0]);

		cout << "Checkpoint 2: " << stopWatch.stop(stopWatch.mainSection)<< "s\n";
		return componentCount;
	}
};


