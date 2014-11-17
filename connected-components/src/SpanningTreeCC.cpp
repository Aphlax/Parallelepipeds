
# include <iostream>
# include <vector>
# include <utility>
# include <algorithm>
# include <queue>

using namespace std;

class SpanningTreeCC {

public:
	/**
	 * returns total number of components and fills out_vertex_to_component filled with each vertex's component.
	 * It expects that outVertexToComponent is a vector of size numberOfVertices, filled with -1
	 */
	int run(const int n, const vector<pair<int,int> > &e, vector<int> &comp) {
		/*		const int m = e.size();
		vector< vector< pair< int, int > > > strees;

		#pragma omp parallel shared(n, m, p, g, e, strees)
		{
			int p = omp_get_num_threads();
			int id = omp_get_thread_num();

			if (id == 0)
				strees = vector< vector< pair< int, int > > >(p);

			#pragma omp barrier

			for (int round = 1; round < p; round *= 2) {
				//get spanning tree for edges id* (m/p) - (id+1)*(m/p)
				vertex< pair<int, int> > stree = alg(n, e);

				strees[id] = stree;
				#pragma omp barrier

				if (id % round == 0) {

					if (id% (round*2) == 0) {

					}
					else {

					}

				}
				else break;
			}



		}

		vector<int> comp(n);
		for (int i = 0; i < n; i++) {
			comp[i] = i;
		}

		for (int i = 0; i < m; i++) {
			int u = edges[i].first;
			int v = edges[i].second;
			u = find(comp, u);
			v = find(comp, v);

			if (u != v) {
				unio(comp, u, v);
				n--;
			}
		}

		cout << n << endl;

		for (int i = 0; i < n; i++) {
			outVertexToComponent[i] = find(comp, i);
		}
*/
		return 0;
	}
};
