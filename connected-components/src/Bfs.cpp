
#include <iostream>
#include <queue>
#include <vector>
#include <ctime>
#include <chrono>

using namespace std;

class Bfs {
	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
	    std::chrono::time_point<std::chrono::system_clock> start, end;
	    std::chrono::duration<double> elapsed_seconds;
	    start = std::chrono::system_clock::now();
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}

		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoint 1: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();

		int componentCount = 0;
		for (unsigned int i = 0; i < graph.size(); ++i) {
			if (outVertexToComponent[i] >= 0) continue;

			int curComponentSize = 0;
			queue<int> q;
			q.push(i);
			while(!q.empty()) {
				int cur = q.front();
				q.pop();
				if (outVertexToComponent[cur] >= 0) continue;
				outVertexToComponent[cur] = componentCount;
				++curComponentSize;

				for (unsigned int j = 0; j < graph[cur].size(); ++j) {
					int next = graph[cur][j];
					if (outVertexToComponent[next] >= 0) continue;
					q.push(next);
				}
			}
			++componentCount;
		}

		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoint 2: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();

		return componentCount;
	}
};
