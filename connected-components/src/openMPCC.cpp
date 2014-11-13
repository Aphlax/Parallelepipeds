// g++ main.cpp BoostCC.cpp RandomGraph.cpp SerialConnectedComponents.cpp openMPCC.cpp -fopenmp


#include <vector>
#include <omp.h>
#include <cmath>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/connected_components.hpp>
#include <iostream>
#include <queue>
#include <set>

using namespace std;
using namespace boost;

class OpenMPCC {

	typedef adjacency_list<vecS, vecS, undirectedS> Graph;
	typedef graph_traits<Graph>::vertex_descriptor Vertex;

	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
		int numThreads = omp_get_num_threads();
		cout << "openMPCC started." << endl;
		std::vector<std::vector<int> > graph(numberOfVertices, std::vector<int>());
		std::vector<set<int> > mergeMap(numberOfVertices, set<int>());
		for (unsigned int i = 0; i < edges.size(); ++i) {
			graph[edges[i].first].push_back(edges[i].second);
			graph[edges[i].second].push_back(edges[i].first);
		}
		#pragma omp parallel for shared(outVertexToComponent)
		for (unsigned int i = 0; i < graph.size(); ++i) {
			if (outVertexToComponent[i] >= 0) continue;
			queue<int> q;
			q.push(i);
			int compMark = i;
			while(!q.empty()) {
				int cur = q.front();
				q.pop();
				if (outVertexToComponent[cur] >= 0) continue;
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

						mergeMap[m].insert(n);
						continue;
					}
					else q.push(next);
				}
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
		return componentCount;
	}
};


