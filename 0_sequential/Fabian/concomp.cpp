/*
 console commands:
g++ -I /home/admin/Desktop/boost_1_54_0 -Wall -O3 -o app concomp.cpp
./app <sample.in >r.txt
diff r.txt sample.out
*/

# include <boost/config.hpp>
# include <boost/graph/adjacency_list.hpp>

# include <iostream>
# include <cassert>
# include <vector>
# include <utility>
# include <algorithm>
# include <queue>
using namespace std;
using namespace boost;

int main ( void ) {

	typedef adjacency_list<vecS, vecS, undirectedS> Graph;
	typedef graph_traits<Graph> Traits;
	typedef Traits::edge_descriptor Edge;
	typedef Traits::out_edge_iterator eIt;
	
	
	ios_base::sync_with_stdio(false);
	
	int n, m;
	cin >> n >> m;
	Graph g(n);
	for (int i = 0; i < m; i++) {
		int u, v;
		cin >> u >> v;
		add_edge(u, v, g);
	}
	
	int compNr = -1, u, v;
	queue<int> q;
	vector<int> visited(n, -1);
	eIt edge_i, edge_end;
	
	for (int i = 0; i < n; i++) {
		if (visited[i] == -1) {
			++compNr;
			q.push(i);
			while (!q.empty()) {
				u = q.front();
				q.pop();
				visited[u] = compNr;
				for(tie(edge_i, edge_end) = out_edges(u, g); edge_i != edge_end; ++edge_i) {
					v = target(*edge_i, g);
					if (visited[v] == -1)
						q.push(v);
				}
			}
		}
	}
	cout << compNr + 1 << endl;
	
	
	for (int i = 0; i < n; i++) {
		cout << i << " " << visited[i] << endl;
	}
	
	return 0;
}























