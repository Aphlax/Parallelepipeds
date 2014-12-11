// g++ main.cpp BoostCC.cpp RandomGraph.cpp SerialConnectedComponents.cpp openMPCC.cpp -fopenmp


#include <vector>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <boost/lockfree/queue.hpp>
#include <tuple>
#include <ctime>
#include <chrono>
#include <atomic>
#include "StopWatch.cpp"


using namespace std;
using namespace boost;

class PBfsAtomic2 {
	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent, StopWatch &stopWatch) {
		//initializing the atomic array
		stopWatch.start(stopWatch.inputProcessing);
		vector< std::atomic<int> > vertexToComponentAtomic(numberOfVertices);
		for(int i=0;i<numberOfVertices;i++)
		{
			vertexToComponentAtomic[i].store(-1);
		}

		//creating the graph
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}
		stopWatch.stop(stopWatch.inputProcessing);
		stopWatch.start(stopWatch.mainSection);

		#pragma omp parallel for
		for (unsigned int i = 0; i < graph.size(); ++i) {
			if (vertexToComponentAtomic[i].load() >= 0) continue;
			std::queue<int> q;
			q.push(i);
			int compMark = i;
			vertexToComponentAtomic[i].store(compMark);
			bool compAlreadyTaken = false;
			while(!q.empty() && !compAlreadyTaken) {
				int cur = q.front(); q.pop();

				for (unsigned int j = 0; j < graph[cur].size(); ++j) {
					int next = graph[cur][j];
					int vertexMark = vertexToComponentAtomic[next].load();
					if (vertexMark!=-1 && vertexMark < compMark)
					{
						compAlreadyTaken = true;
						break;
					}
					else if(vertexMark!= compMark)
					{
						vertexToComponentAtomic[next].store(compMark);
						q.push(next);
					}
				}
			}
		}
		for(int i=0;i<numberOfVertices;i++)
		{
			outVertexToComponent[i] = vertexToComponentAtomic[i];
		}
		stopWatch.stop(stopWatch.mainSection);
		return 0;
	}
};


