// g++ main.cpp BoostCC.cpp RandomGraph.cpp SerialConnectedComponents.cpp openMPCC.cpp -fopenmp


#include <vector>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <algorithm>

using namespace std;

class OpenMPRandomizedContractingCC {

	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
		cout << "RandomizedContractingMPCC started" << endl;
		vector<int> C = vector<int>(numberOfVertices);
		vector<int> M = vector<int>(numberOfVertices);
		vector<int> S = vector<int>(edges.size());

		vector<int> test = vector<int>(50);
		#pragma omp parallel default(none) shared(test)
		{
			unsigned int seed = omp_get_thread_num();
			#pragma omp for
			for (unsigned int i = 0; i < test.size(); ++i) {
				test[i] = rand_r(&seed);
			}
		}
		sort(test.begin(), test.end());
		for (unsigned int i = 0; i < test.size(); ++i) {
			cout << test[i] << endl;
		}
//		for (unsigned int i = 0; i < graph.size(); ++i) {
//
//		}



//		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
//		std::vector<std::vector<int> > mergeMap(numberOfVertices, std::vector<int>());
//		for (unsigned int i = 0; i < edges.size(); ++i) {
//			graph[edges[i].first].push_back(edges[i].second);
//			graph[edges[i].second].push_back(edges[i].first);
//		}
//
//
//		int componentCount = 0;
//		#pragma omp parallel for shared(outVertexToComponent) reduction(+:componentCount)
//		for (unsigned int i = 0; i < graph.size(); ++i) {
//			if (outVertexToComponent[i] >= 0) continue;
//			queue<int> q;
//			q.push(i);
//			while(!q.empty()) {
//				int cur = q.front();
//				q.pop();
//				if (outVertexToComponent[cur] >= 0) continue;
//				outVertexToComponent[cur] = componentCount;
//
//				for (unsigned int j = 0; j < graph[cur].size(); ++j) {
//					int next = graph[cur][j];
//					if (outVertexToComponent[next] >= 0)
//					{
//						mergeMap[cur].push_back(outVertexToComponent[next]);
//						continue;
//					}
//					q.push(next);
//				}
//			}
//		}
//		cout << "done"<< endl;
//		componentCount = 2;
		return 0;
	}
};


