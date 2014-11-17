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


using namespace std;
using namespace boost;

class OpenMPCC {

	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
		std::chrono::time_point<std::chrono::system_clock> start, end;
		start = std::chrono::system_clock::now();

		cout << "openMPCC started." << endl;
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}

		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		cout << "graph generating time: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();

		std::vector<set<int> > mergeMapArray[10] ;
		//std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
		int nt;
		#pragma omp parallel //shared(outVertexToComponent)
		{
			int tn = omp_get_thread_num();
			nt = omp_get_num_threads();
			std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
			mergeMapArray[tn] = mergeMap;
			#pragma omp for
			for (unsigned int i = 0; i < graph.size(); ++i) {
				if (outVertexToComponent[i] >= 0) continue;
				queue<int> q;
				q.push(i);
				int compMark = i;
				while(!q.empty()) {
					int cur = q.front();
					q.pop();
					if (outVertexToComponent[cur] == compMark) continue;
					outVertexToComponent[cur] = compMark;
					for (unsigned int j = 0; j < graph[cur].size(); ++j) {
						int next = graph[cur][j];
						if (outVertexToComponent[next] >= 0)
						{
							int m,n;
							if(compMark>outVertexToComponent[next])
							{	m= outVertexToComponent[next]; n = compMark;}
							else
							{	n= outVertexToComponent[next]; m = compMark;}

							//mergeMap[m].insert(n);
							mergeMapArray[tn][m].insert(n);
							continue;
						}
						else q.push(next);
					}
				}
			}
		}

		end = std::chrono::system_clock::now();
		elapsed_seconds = end-start;
		cout << "Main time: " << elapsed_seconds.count() << "s\n";


		start = std::chrono::system_clock::now();

		std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
		//merge the mergeMaps
		for(int i=0;i<nt;i++)
		{
			for(int j=0;j<graph.size();j++)
			{
				mergeMap[j].insert(mergeMapArray[i][j].begin(), mergeMapArray[i][j].end());
			}
		}

		vector<int> countPerComp(numberOfVertices,0);
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			countPerComp[outVertexToComponent[i]]++;
		}
		//clean the merging map
		vector<int>finalCompNr(numberOfVertices,-1);
		int componentCount=0;
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			if(countPerComp[i]>0 && finalCompNr[i]<0)
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

		end = std::chrono::system_clock::now();
		elapsed_seconds = end-start;
		cout << "merge time: " << elapsed_seconds.count() << "s\n";
		return componentCount;
	}
};

