// g++ main.cpp BoostCC.cpp RandomGraph.cpp SerialConnectedComponents.cpp openMPCC.cpp -fopenmp

//time pbfsatomic2, graph04, takes 18 sec.
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
			if(vertexToComponentAtomic[i].load()!=-1)continue;
			//vertexToComponentAtomic[i].store(i);
			//if(vertexToComponentAtomic[i].load()!=i)continue;
			int testValue = -1;
			if(!vertexToComponentAtomic[i].compare_exchange_strong(testValue,i,std::memory_order_relaxed))continue;
			std::queue<int> q;
			q.push(i);
			int compMark = i;
			bool compAlreadyTaken = false;
			while(!q.empty() && !compAlreadyTaken) {
				int cur = q.front(); q.pop();
				for (unsigned int j = 0; j < graph[cur].size(); ++j) {
					int next = graph[cur][j];
					int nextMark;
					testValue =-1;

					//vertex is set and component has smaller index than compMark ==> abort
					if ((nextMark = vertexToComponentAtomic[next].load()) < compMark && nextMark!=-1)
					{
						compAlreadyTaken = true;
						break;
					}
					//vertex is not set yet
					else if(vertexToComponentAtomic[next].compare_exchange_strong(testValue,compMark,std::memory_order_relaxed))
					{
						q.push(next);
					}
					//vertex is (hopefully) higher, try setting your (lower) index (overtaking that component)
					else
					{
						bool exchanged=false;
						do
						{
							nextMark = vertexToComponentAtomic[next].load();
							if(compMark<nextMark)
							{
								if((exchanged = vertexToComponentAtomic[next].compare_exchange_strong(nextMark,compMark,std::memory_order_relaxed)))
								{
									q.push(next);
								}
							}
							else
								exchanged=true;
						}while(!exchanged);
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


