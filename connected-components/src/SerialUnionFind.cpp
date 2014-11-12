
#include "SerialUnionFind.h"

# include <iostream>
# include <vector>
# include <utility>
# include <algorithm>
# include <queue>

using namespace std;

// Returns the set of that element
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

int SerialUnionFind::run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
	int n = numberOfVertices;
	int m = edges.size();
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

	for (int i = 0; i < n; i++) {
		outVertexToComponent[i] = find(comp, i);
	}

	return n;
}





