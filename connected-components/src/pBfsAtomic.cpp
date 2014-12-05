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


using namespace std;
using namespace boost;

class PBfsAtomic {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;

	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {

		cout << "pBfsAtomic started." << endl;

		//initializing the atomic array
		start = std::chrono::system_clock::now();
		vector< std::atomic<int> > vertexToComponentAtomic(numberOfVertices);
		for(int i=0;i<numberOfVertices;i++)
		{
			vertexToComponentAtomic[i].store(-1);
		}
		end = std::chrono::system_clock::now();
		elapsed_seconds = end-start;
		cout << "initializing atomic array time: " << elapsed_seconds.count() << endl;

		//creating the graph
		start = std::chrono::system_clock::now();
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		int nt;
		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}
		end = std::chrono::system_clock::now();
		elapsed_seconds = end-start;
		cout << "graph generating time: " << elapsed_seconds.count() << endl;

		//connected component alogrithm
		start = std::chrono::system_clock::now();
		std::vector<set<int> > mergeMapArray[10] ;

		#pragma omp parallel //shared(outVertexToComponent)
		{
			int tn = omp_get_thread_num();
			nt = omp_get_num_threads();
			std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
			mergeMapArray[tn] = mergeMap;
			std::queue<int> q;
			#pragma omp for
			for (unsigned int i = 0; i < graph.size(); ++i) {
				if (vertexToComponentAtomic[i].load() >= 0) continue;
				q.push(i);
				int compMark = i;
				vertexToComponentAtomic[i].store(compMark);
				while(!q.empty()) {
					int cur = q.front(); q.pop();

					for (unsigned int j = 0; j < graph[cur].size(); ++j) {
						int next = graph[cur][j];
						int vertexMark = vertexToComponentAtomic[next].load();
						if (vertexMark >= 0 && vertexMark!= compMark)
						{
							mergeMapArray[tn][compMark].insert(vertexMark);
							mergeMapArray[tn][vertexMark].insert(compMark);
							continue;
						}
						else if(vertexMark!= compMark)
						{
							vertexToComponentAtomic[next].store(compMark);
							q.push(next);
						}
					}
				}
			}
		}
		end = std::chrono::system_clock::now();
		elapsed_seconds = end-start;
		cout << "Main time: " << elapsed_seconds.count() << endl;


		start = std::chrono::system_clock::now();

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
			compHasElem[vertexToComponentAtomic[i]]++;
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
			outVertexToComponent[i]  = finalCompNr[vertexToComponentAtomic[i]];
		}

		end = std::chrono::system_clock::now();
		elapsed_seconds = end-start;
		cout << "merge time: " << elapsed_seconds.count() << "s\n";
		return componentCount;
	}
};


