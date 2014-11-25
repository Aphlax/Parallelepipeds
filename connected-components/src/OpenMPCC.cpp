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
		int nt;
		//#pragma omp parallel
		{
			//int tn = omp_get_thread_num();
			//nt = omp_get_num_threads();
			int size = edges.size();
			for (unsigned int i = 0; i < size; ++i) {
				graph[edges[i].first].push_back(edges[i].second);
				graph[edges[i].second].push_back(edges[i].first);
			}
		}
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end-start;
		cout << "graph generating time: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();

		std::vector<set<int> > mergeMapArray[10] ;
		//std::vector<set<int> > mergeMap(numberOfVertices, set<int>());


		int size = graph.size();
		#pragma omp parallel //shared(outVertexToComponent)
		{
			int tn = omp_get_thread_num();
			nt = omp_get_num_threads();
			int workLoad = size/nt;
			std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
			std::vector<int> beginningNode;
			mergeMapArray[tn] = mergeMap;
			queue<int> q;
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
							int m = min(compMark, outVertexToComponent[next]);
							int n = max(compMark, outVertexToComponent[next]);
							//if(m==0)cout << "merge o with "<< n << endl;
							//if(next==2855)cout << "merge 2855 with "<< n << " or " << m << endl;
							mergeMapArray[tn][m].insert(n);
							continue;
						}
						else if(outVertexToComponent[next]!= compMark)
						{
							//if(compMark==0)cout << next << endl;

							outVertexToComponent[next] = compMark;
							q.push(next);
						}
					}
				}
			}
			for(int i=0;i<beginningNode.size();i++)
			{
				if(beginningNode[i]!= outVertexToComponent[beginningNode[i]])
				{
					int m,n;
					if(beginningNode[i]>outVertexToComponent[i])
					{	m= outVertexToComponent[i]; n = beginningNode[i];}
					else
					{	n= outVertexToComponent[i]; m = beginningNode[i];}
					mergeMapArray[tn][m].insert(n);
					//cout << "found "<< m << " " << n << endl;
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

		vector<bool> compHasElem(numberOfVertices,false);

		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			compHasElem[outVertexToComponent[i]]=true;
		}
		//clean the merging map
		vector<int>finalCompNr(numberOfVertices,-1);
		int componentCount=0;
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			if(compHasElem[i] && finalCompNr[i]<0)
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


