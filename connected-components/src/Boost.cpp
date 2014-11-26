#include <vector>
#include <omp.h>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/connected_components.hpp>
#include <ctime>
#include <chrono>

using namespace std;
using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;

class Boost {

public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
		std::chrono::time_point<std::chrono::system_clock> start, end;
		std::chrono::duration<double> elapsed_seconds;
		start = std::chrono::system_clock::now();

		Graph g(numberOfVertices);
		for (unsigned int i = 0; i < edges.size(); ++i) {
			add_edge(edges[i].first, edges[i].second,g);
		}

		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoint 1: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();

		int componentCount = connected_components(g, &outVertexToComponent[0]);

		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoint 2: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();
		return componentCount;
	}
};


