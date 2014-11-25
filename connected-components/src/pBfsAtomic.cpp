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
		cout << "initializing atomic array time: " << elapsed_seconds.count() << "s\n";

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
		cout << "graph generating time: " << elapsed_seconds.count() << "s\n";

		//connected component alogrithm
		start = std::chrono::system_clock::now();
		std::vector<set<int> > mergeMapArray[10] ;
		int counter=0;
		int countMarks = 0;
		#pragma omp parallel //shared(outVertexToComponent)
		{
			int tn = omp_get_thread_num();
			nt = omp_get_num_threads();
			std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
			mergeMapArray[tn] = mergeMap;
			queue<int> q;
			#pragma omp for
			for (int i = 0; i < graph.size(); ++i) {
				if (vertexToComponentAtomic[i].load() >= 0) continue;
				q.push(i);
				int compMark = i;
				#pragma omp atomic
				countMarks++;
				vertexToComponentAtomic[i].store(compMark);
				while(!q.empty()) {
					int cur = q.front(); q.pop();

					for (unsigned int j = 0; j < graph[cur].size(); ++j) {
						int next = graph[cur][j];
						int vertexMark = vertexToComponentAtomic[next].load();
						if (vertexMark >= 0 && vertexMark!= compMark)
						{
							int m = min(compMark, vertexMark);
							int n = max(compMark, vertexMark);
							//if(m==0)cout << "merge o with "<< n << endl;
							//if(next==2855)cout << "merge 2855 with "<< n << " or " << m << endl;
							if(mergeMapArray[tn][m].find(n)==mergeMapArray[tn][m].end())
							{
								#pragma omp atomic
								counter ++;
							}
							mergeMapArray[tn][m].insert(n);
							continue;
						}
						else if(vertexMark!= compMark)
						{
							//if(compMark==0)cout << next << endl;

							vertexToComponentAtomic[next].store(compMark);
							q.push(next);
						}
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
		int counter2=0;
		for(int i=0;i<nt;i++)
		{
			for(int j=0;j<graph.size();j++)
			{
				mergeMap[j].insert(mergeMapArray[i][j].begin(), mergeMapArray[i][j].end());
				counter2+= mergeMapArray[i][j].size();
			}
		}
		cout << "counters: " << counter << " and " << counter2 << "marks" << countMarks << endl;
		cout << "comp 0 has " <<  mergeMap[0].size() << " entires in the merge map" << endl;
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
					if(not mergeMap[*it].empty())
					{
						mergeSet.insert(mergeMap[*it].begin(),mergeMap[*it].end());
						mergeMap[*it].clear();
						newAdded=true;
					}
				}
				mergeMap[i].insert(mergeSet.begin(), mergeSet.end());
			}while(newAdded);
		}

		cout << "after: comp 0 has " <<  mergeMap[0].size() << " entires in the merge map" << endl;
		//assign final comp nr
		vector<bool> compHasElem(numberOfVertices,false);
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			compHasElem[vertexToComponentAtomic[i]]=true;
		}
		vector<int>finalCompNr(numberOfVertices,-1);
		int componentCount=0;
		for(unsigned int i = 0; i < graph.size(); ++i)
		{
			if(not mergeMap[i].empty())
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


