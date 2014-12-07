
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <queue>
#include <omp.h>
#include "StopWatch.cpp"

using namespace std;

class SpanningTreeCC {

	// Returns the set of that element, and does path-halving
	inline int find(vector<int> &comp, int find) {
		int last = -1;
		while (comp[find] != find) {
			if (last != -1)
				comp[last] = comp[find];
			last = find;
			find = comp[find];
		}
		return find;
	}

	// Unions two sets
	inline void unio(vector<int> &comp, int u, int v) {
		if (u > v)
			comp[u] = v;
		else
			comp[v] = u;
	}

	/**
	 * returns total number of components and fills out_vertex_to_component filled with each vertex's component.
	 * It expects that outVertexToComponent is a vector of size numberOfVertices, filled with -1
	 */
public:
	int run(const int n, const vector<pair<int,int> > &e, vector<int> &outVertexToComponent) {
		const int m = e.size();
		vector<vector<pair<int, int> >* > strees;
		vector<int> streeSize;
		vector<int> resultComp;
		int resultSize;
		StopWatch stopWatch;
		stopWatch.start(stopWatch.mainSection);
		#pragma omp parallel shared(strees, streeSize, resultComp, resultSize)
		{
			// Initialisation

			const int p = omp_get_num_threads();
			const int id = omp_get_thread_num();
			int round = 2;
			vector<int> comp(n);
			vector<pair<int, int> > stree(n);
			int streeCount = 0;

			for (int i = 0; i < n; i++) {
				comp[i] = i;
			}

			if (id == 0) {
				strees = vector<vector< pair< int, int > >* >(p, nullptr);
				streeSize = vector<int>(p, -1);
			}

			#pragma omp barrier // First round

			{
				// split up edges: each thread processes a subset of the edges
				int s0 = id * (m / p);
				int e0 = id == p - 1 ? m : (id + 1)*(m / p);

				// run union find for these edges and keep a spanning tree up to date
				// (thread 0 does not need that, only needed for sending results to other threads.)
				for (int i = s0; i < e0; i++) {
					int u = e[i].first;
					int v = e[i].second;
					u = find(comp, u);
					v = find(comp, v);

					if (u != v) {
						unio(comp, u, v);

						if (id != 0)
							stree[streeCount++] = e[i];
						else
							streeCount++;
					}
				}
			}

			// All other rounds

			for (; round < 2 * p; round *= 2) {
				if (id % round == 0) {
					#pragma omp barrier
					// 'receive' spanning tree from other thread and add the edges to the current result
					// (by continuing union find with these additional edges.)
					int otherId = id + round / 2;
					if (otherId < p) {// this check is needed if #processors is not a power of 2
						vector<pair<int, int> > otherTree = *strees[otherId];
						int otherCount = streeSize[otherId];

						for (int i = 0; i < otherCount; i++) {
							int u = otherTree[i].first;
							int v = otherTree[i].second;
							u = find(comp, u);
							v = find(comp, v);

							if (u != v) {
								unio(comp, u, v);

								if (id != 0)
									stree[streeCount++] = otherTree[i];
								else
									streeCount++;
							}
						}
					}
				} else {// Thread is finished, share results and go to barrier
					strees[id] = &stree;
					streeSize[id] = streeCount;

					#pragma omp barrier

					break;
				}
			}

			// those who fell out before the final round still have to wait for barriers
			for (; round < 2 * p; round *= 2) {
				#pragma omp barrier
			}

			// Thread 0 has the results
			if (id == 0) {
				resultComp = comp;
				resultSize = n - streeCount;
			}
		}//end omp parallel

		for (int i = 0; i < n; i++) {
			outVertexToComponent[i] = find(resultComp, i);
		}

		stopWatch.stop(stopWatch.mainSection);
		return resultSize;
	}

};
