
#include <iostream>
#include <stack>
#include <stack>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <omp.h>
#include "StopWatch.cpp"

using namespace std;

class PBfs {
public:


	int run(const int numberOfVertices, const std::vector<std::pair<int, int> > &edges, std::vector<int> &outVertexToComponent, StopWatch &stopWatch) {
		stopWatch.start(stopWatch.inputProcessing);
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}
		stopWatch.stop(stopWatch.inputProcessing);

		runBfs(numberOfVertices, graph, outVertexToComponent, stopWatch, -1);

		// correctness check
		bool incorrect = false;
		#pragma omp parallel for default(none) shared(edges, outVertexToComponent, incorrect)
		for (unsigned int i = 0; i < edges.size(); ++i) {
			if (outVertexToComponent[edges[i].first] != outVertexToComponent[edges[i].second]) {
				incorrect = true;
			}
		}
		// correctness safeguard
		if (incorrect) {
			cout << "Parallel processing yielded incorrect result. Re-running with 1 thread\n";
			#pragma omp parallel for default(none) shared(outVertexToComponent)
			for (unsigned int i = 0; i < outVertexToComponent.size(); ++i) outVertexToComponent[i] = -1;
			runBfs(numberOfVertices, graph, outVertexToComponent, stopWatch, 1);
		}

		return 0;
	}


private:

	void runBfs(const int numberOfVertices, const std::vector<std::vector<int> > &graph, std::vector<int> &outVertexToComponent, StopWatch &stopWatch, int maxThreads) {
		int prevMaxThreads = omp_get_max_threads();
		if (maxThreads != -1) {
			omp_set_num_threads(maxThreads);
		}

		stopWatch.start(stopWatch.mainSection);
		std::vector<std::unordered_map<int, int> > componentHits(omp_get_max_threads(), std::unordered_map<int, int>());
		#pragma omp parallel default(none) shared(graph, componentHits, outVertexToComponent)
		{
			int tn = omp_get_thread_num();

			stack<int> s;
			int componentCount = tn * numberOfVertices;
			#pragma omp for
			for (unsigned int i = 0; i < graph.size(); ++i) {
				if (outVertexToComponent[i] >= 0) continue;
				s.push(i);
				bool marked1 = false;
				while (!s.empty()) {
					int cur = s.top();
					s.pop();
					int curComponent = outVertexToComponent[cur];
					if (curComponent == componentCount) continue;
					else if (curComponent >= 0) {
						if (!marked1) continue; // havent even marked any in this component, no need to mark merging stuff

						addToMap(componentHits[tn], curComponent, componentCount);
						continue;
					}
					marked1 = true;
					outVertexToComponent[cur] = componentCount;

					for (unsigned int j = 0; j < graph[cur].size(); ++j) {
						int next = graph[cur][j];
						if (outVertexToComponent[next] == componentCount)
							continue;
						s.push(next);
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

		if (maxThreads != -1) {
			omp_set_num_threads(prevMaxThreads);
		}

		#pragma omp parallel for default(none) shared(graph, componentHits, outVertexToComponent, map)
		for (unsigned int i = 0; i < graph.size(); ++i) {
			int component = outVertexToComponent[i];
			if (component < numberOfVertices) continue;
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
