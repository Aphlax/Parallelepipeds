
#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <omp.h>
#include <atomic>
#include "StopWatch.cpp"

using namespace std;

class PBfsAtomic {
public:


	int run(const int numberOfVertices, const std::vector<std::pair<int, int> > &edges, std::vector<int> &outVertexToComponent, StopWatch &stopWatch) {
		stopWatch.start(stopWatch.inputProcessing);
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}
		stopWatch.stop(stopWatch.inputProcessing);

		runBfs(numberOfVertices, graph, outVertexToComponent, stopWatch);

		return 0;
	}


private:

	void runBfs(const int numberOfVertices, const std::vector<std::vector<int> > &graph, vector<int> &outVertexToComponent, StopWatch &stopWatch) {
		std::atomic_flag atomicFlags[numberOfVertices];
		stopWatch.start(stopWatch.mainSection);
		std::vector<std::unordered_map<int, int> > componentHits(omp_get_max_threads(), std::unordered_map<int, int>());
		#pragma omp parallel default(none) shared(graph, componentHits, outVertexToComponent, atomicFlags)
		{
			int tn = omp_get_thread_num();

			queue<int> q;
			int componentCount = tn * numberOfVertices;
			#pragma omp for
			for (unsigned int i = 0; i < graph.size(); ++i) {
				if (outVertexToComponent[i] >= 0) continue;
				q.push(i);
				bool marked1 = false;
				while (!q.empty()) {
					int cur = q.front();
					q.pop();
					int curComponent = outVertexToComponent[cur];
					if (curComponent == componentCount) continue;
					else if (curComponent >= 0) {
						if (!marked1) continue; // havent even marked any in this component, no need to mark merging stuff

						addToMap(componentHits[tn], curComponent, componentCount);
						continue;
					}

					if (atomicFlags[cur].test_and_set()) {
						// already set, push back vertex and try again later
						q.push(cur);
					}

					marked1 = true;
					outVertexToComponent[cur] = componentCount;

					for (unsigned int j = 0; j < graph[cur].size(); ++j) {
						int next = graph[cur][j];
						if (outVertexToComponent[next] == componentCount)
							continue;
						q.push(next);
					}
				}
				if (marked1)
					++componentCount;
			}
		}
		stopWatch.stop(stopWatch.mainSection);
		stopWatch.start(stopWatch.merging);
		std::unordered_map<int, int> map;
		for (unsigned int i = 0; i < componentHits.size(); ++i) {
			for (std::unordered_map<int, int>::iterator it = componentHits[i].begin(); it != componentHits[i].end(); ++it) {
				addToMap(map, it->first, it->second);
			}
		}

		#pragma omp parallel for default(none) shared(graph, componentHits, outVertexToComponent, map)
		for (unsigned int i = 0; i < graph.size(); ++i) {
			int component = outVertexToComponent[i];
			std::unordered_map<int, int>::iterator it = map.find(component);
			while (it != map.end()) {
				component = it->second;
				it = map.find(component);
			}
			outVertexToComponent[i] = component;
		}

		stopWatch.stop(stopWatch.merging);
	}

	void addToMap(std::unordered_map<int, int> &map, int n1, int n2) {
		int a = max(n1, n2);
		int b = min(n1, n2);
		while (true) {
			std::unordered_map<int, int>::iterator it = map.find(a);
			if (it == map.end()) {
				map.insert(pair<int, int>(a, b));
				return;
			}
			int c = it->second;
			if (b == c)	return;
			if (b > c) {
				map.insert(pair<int, int>(b, c));
				return;
			}
			a = c;
		}

	}
};
