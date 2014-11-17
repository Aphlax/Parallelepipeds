#include <vector>
#include <omp.h>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>


using namespace std;
using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;


class pBoost {

public:
	/**
	 * returns total number of components and fills out_vertex_to_component filled with each vertex's component.
	 * It expects that outVertexToComponent is a vector of size numberOfVertices, filled with -1
	 */
	int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {

		return 0;
	}
};

