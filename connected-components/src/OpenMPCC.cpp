// g++ main.cpp BoostCC.cpp RandomGraph.cpp SerialConnectedComponents.cpp openMPCC.cpp -fopenmp


#include <vector>
#include <omp.h>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/connected_components.hpp>
#include <iostream>
#include <queue>

using namespace std;
using namespace boost;

class OpenMPCC {

	typedef adjacency_list<vecS, vecS, undirectedS> Graph;
	typedef graph_traits<Graph>::vertex_descriptor Vertex;

	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
		int numThreads = omp_get_num_threads();
		cout << "openMPCC started" << endl;
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		std::vector<std::vector<int> > mergeMap(numberOfVertices, std::vector<int>());
		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}


		int componentCount = 0;
		#pragma omp parallel for shared(outVertexToComponent) reduction(+:componentCount)
		for (unsigned int i = 0; i < graph.size(); ++i) {
			if (outVertexToComponent[i] >= 0) continue;
			queue<int> q;
			q.push(i);
			while(!q.empty()) {
				int cur = q.front();
				q.pop();
				if (outVertexToComponent[cur] >= 0) continue;
				outVertexToComponent[cur] = componentCount;

				for (unsigned int j = 0; j < graph[cur].size(); ++j) {
					int next = graph[cur][j];
					if (outVertexToComponent[next] >= 0)
					{
						mergeMap[cur].push_back(outVertexToComponent[next]);
						continue;
					}
					q.push(next);
				}
			}
		}
		cout << "done"<< endl;
		componentCount = 2;
		return componentCount;
	}
};


