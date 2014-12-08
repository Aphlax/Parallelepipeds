// g++ main.cpp BoostCC.cpp RandomGraph.cpp SerialConnectedComponents.cpp openMPCC.cpp -fopenmp


#include <vector>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <boost/lockfree/queue.hpp>
#include <tuple>
#include "StopWatch.cpp"

using namespace std;
using namespace boost;

class OpenMPCC {
	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent, StopWatch &stopWatch) {
		stopWatch.start(stopWatch.inputProcessing);
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		int nt;

		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}
		stopWatch.stop(stopWatch.inputProcessing);
		stopWatch.start(stopWatch.mainSection);
		std::vector<set<int> > mergeMapArray[10] ;

		int size = graph.size();
		#pragma omp parallel //shared(outVertexToComponent)
		{
			int tn = omp_get_thread_num();
			nt = omp_get_num_threads();
			int workLoad = size/nt;
			std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
			std::vector<int> beginningNode;
			mergeMapArray[tn] = mergeMap;
			std::queue<int> q;
			//#pragma omp for
			for (int i = tn*workLoad; i < (tn+1)*workLoad; ++i) {
				if (outVertexToComponent[i] >= 0) continue;
				beginningNode.push_back(i);
				q.push(i);
				int compMark = i;
				outVertexToComponent[i] = compMark;
				while(!q.empty()) {
					int cur = q.front(); q.pop();

					for (unsigned int j = 0; j < graph[cur].size(); ++j) {
						int next = graph[cur][j];
						if (outVertexToComponent[next] >= 0 && outVertexToComponent[next]!= compMark)
						{
							int m = compMark;
							int n = outVertexToComponent[next];
							mergeMapArray[tn][m].insert(n);
							mergeMapArray[tn][n].insert(m);
							continue;
						}
						else if(outVertexToComponent[next]!= compMark)
						{
							outVertexToComponent[next] = compMark;
							q.push(next);
						}
					}
				}
			}
		}
		stopWatch.stop(stopWatch.mainSection);
		stopWatch.start(stopWatch.merging);
		std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
		//merge the mergeMaps
		for(int i=0;i<nt;i++)
		{
			for(unsigned int j=0;j<graph.size();j++)
			{
				mergeMap[j].insert(mergeMapArray[i][j].begin(), mergeMapArray[i][j].end());
			}
		}
		//clean the merging map
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			bool newAdded=false;
			do
			{
				newAdded=false;
				set<int> mergeSet;
				//#pragma omp parallel for
				for(set<int>::iterator it = mergeMap[i].begin(); it!=mergeMap[i].end() ;it++)
				{
					if(not mergeMap[*it].empty() && *it != (int)i)
					{
						mergeSet.insert(mergeMap[*it].begin(),mergeMap[*it].end());
						mergeMap[*it].clear();
						newAdded=true;
					}
				}
				mergeMap[i].insert(mergeSet.begin(), mergeSet.end());
			}while(newAdded);
		}
		//assign final comp nr
		vector<int> compHasElem(numberOfVertices,0);
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			compHasElem[outVertexToComponent[i]]++;
		}
		vector<int>finalCompNr(numberOfVertices,-1);
		int componentCount=0;
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			if(compHasElem[i]>0 && finalCompNr[i]<0)
			{
				finalCompNr[i]=componentCount;
				componentCount++;
			}
			for(set<int>::iterator it = mergeMap[i].begin(); it!=mergeMap[i].end() ;it++)
			{
				finalCompNr[*it] = finalCompNr[i];
			}
		}

		//mark all vertices with the new Index
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			outVertexToComponent[i]  = finalCompNr[outVertexToComponent[i]];
		}
		stopWatch.stop(stopWatch.merging);
		return componentCount;
	}
};


