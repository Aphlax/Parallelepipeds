
# include <iostream>
# include <vector>
# include <utility>
# include <algorithm>
# include <queue>
# include <omp.h>

# include "SerialConnectedComponents.h"

using namespace std;

class SpanningTreeCC {

	// Returns the set of that element, and does path-halving
	inline int find(vector<int> comp, int find) {
		int last = -1;
		while (comp[find] != find) {
			if (last != -1)
				comp[last] = comp[find];
			last = find;
			find = comp[find];
		}
	}

	// Unions two sets
	inline void unio(vector<int> comp, int u, int v) {
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
		vector<vector<pair<int, int> > > strees;
		vector<int> streeSize;
		vector<int> resultComp;
		int resultSize;

		#pragma omp parallel shared(strees, streeSize, resultComp, resultSize)
		{
			int p = omp_get_num_threads();
			const int id = omp_get_thread_num();
			int round = 2;
			vector<int> comp(n);
			vector<pair<int, int> > stree(n);
			int streeCount = 0;

			for (int i = 0; i < n; i++) {
				comp[i] = i;
			}

			if (id == 0) {
				cout << "#threads: " << p << "   #nodes: " << n  << "   #edges: " << m << endl;
				strees = vector<vector< pair< int, int > > >(p);
				streeSize = vector<int>(p, -1);
			}

			#pragma omp barrier

			{
				int s0 = id*(m / p);
				int e0 = id == p -1 ? m : (id+1)*(m / p);


				for (int i = s0; i < e0; i++) {
					int u = e[i].first;
					int v = e[i].second;
					u = find(comp, u);
					v = find(comp, v);

					if (u != v) {
						unio(comp, u, v);

						if (id != 0)
							stree[streeCount++] = e[i];
					}
				}
			}

			#pragma omp barrier


			for (; round < p; round *= 2) {
				if (id % round == 0) {
					#pragma omp barrier
					int otherId = id + round / 2;
					if (otherId < p) {
						vector<pair<int, int> > otherTree = strees[otherId];
						int otherCount = streeSize[otherId];

						for (int i = 0; i < otherCount; i++) {
							int u = otherTree[i].first;
							int v = otherTree[i].second;
							u = find(comp, u);
							v = find(comp, v);

							if (u != v) {
								unio(comp, u, v);

								if (id != 0)
									stree[streeCount++] = e[i];
							}
						}
					}
				}
				else {
					strees[id] = stree;
					streeSize[id] = streeCount;

					#pragma omp barrier

					break;
				}

			}
			for (; round < p; round *= 2) {
				#pragma omp barrier
			}

			if (id == 0) {
				resultComp = comp;
				resultSize = n - streeCount;
			}
		}

		for (int i = 0; i < n; i++) {
			outVertexToComponent[i] = find(resultComp, i);
		}

		return resultSize;
	}


	int alg(const int n, int s0, int e0, const vector< pair<int, int> > &edges0, int e1, vector<pair<int,int> > &edges1, vector< pair<int, int> > &stree) {

		return 0;

	}

	/*// Old version with BFS - slow and wrong
public:
	int run(const int n, const vector<pair<int,int> > &e, vector<int> &comp) {
		const int m = e.size();
		vector<vector<pair<int, int> > > strees;
		vector<int> streeSize;

		#pragma omp parallel shared(strees, streeSize)
		{
			int p = omp_get_num_threads();
			int id = omp_get_thread_num();
			int round = 2;

			if (id == 0) {
				cout << "Number of threads: " << p << endl;
				cout << "Number of nodes: " << n << endl;
				cout << "Number of edges: " << m << endl;
				strees = vector< vector< pair< int, int > > >(p);
				streeSize = vector<int>(p);
			}

			#pragma omp barrier

			{
				int s0 = id*(m / p);
				int e0 = id == p -1 ? m : (id+1)*(m / p);
				vector<pair<int, int> > stree(n);

				cout << "Thread " << id << " is handling edges " << s0 << " - " << e0 << endl;

				streeSize[id] = alg(n, s0, e0, e, 0 , stree, stree);

				strees[id] = stree;
			}

			#pragma omp barrier


			for (; round < p; round *= 2) {

				#pragma omp barrier
				if (id % round == 0) {
					int s0 = 0;
					int e0 = streeSize[id];
					vector<pair<int, int> > edges0 = strees[id];
					int e1 = streeSize[id + round/2];
					vector<pair<int, int> > edges1 = strees[id + round/2];
					vector<pair<int, int> > stree(n);

					streeSize[id] = alg(n, s0, e0, edges0, e1, edges1, stree);

					strees[id] = stree;
				}
				else break;
			}
			for (; round < p; round *= 2) {
				#pragma omp barrier
			}
		}

		SerialConnectedComponents cc;
		return cc.run(n, strees[0], comp);
	}

	int alg(const int n, int s0, int e0, const vector< pair<int, int> > &edges0, int e1, vector<pair<int,int> > &edges1, vector< pair<int, int> > &stree) {
		vector<vector<int> > graph(n, vector<int>());
		for (unsigned int i = s0; i < e0; ++i) {
			graph[edges0[i].first].push_back(edges0[i].second);
			graph[edges0[i].second].push_back(edges0[i].first);
		}
		for (unsigned int i = 0; i < e1; ++i) {
			graph[edges1[i].first].push_back(edges1[i].second);
			graph[edges1[i].second].push_back(edges1[i].first);
		}


		int nre = 0;
		vector<bool> vertexToComponent(n, false);
		queue<pair<int, int> > q;
		for (unsigned int i = 0; i < n; ++i) {
			if (vertexToComponent[i]) continue;
			vertexToComponent[i] = true;

			for (unsigned int j = 0; j < graph[i].size(); ++j) {
				int next = graph[i][j];
				q.push(make_pair(i, next));
			}
			while(!q.empty()) {
				pair<int, int> p = q.front();
				int cur = p.second;
				q.pop();
				if (vertexToComponent[cur]) continue;
				vertexToComponent[cur] = true;
				stree[nre++] = p;

				for (unsigned int j = 0; j < graph[cur].size(); ++j) {
					int next = graph[cur][j];
					if (vertexToComponent[next]) continue;
					q.push(make_pair(cur, next));
				}
			}
		}
		cout << nre << endl;

		return nre;
	}//*/
};
