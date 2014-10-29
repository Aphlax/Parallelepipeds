/*
 console commands:
g++ -I /home/admin/Desktop/boost_1_54_0 -Wall -O3 -o app concomp.cpp
./app <sample.in >r.txt
diff r.txt sample.out
*/

# include <iostream>
# include <cassert>
# include <vector>
# include <utility>
# include <algorithm>
# include <queue>
using namespace std;
using namespace boost;

int main ( void ) {	
	
	ios_base::sync_with_stdio(false);
	
	int n, m;
	cin >> n >> m;
	vector<int> comp(n);
	for (int i = 0; i < n; i++) {
		comp[i] = i;
	}
	
	for (int i = 0; i < m; i++) {
		int u, v;
		cin >> u >> v;
		u = find(comp, u)
		v = find(comp, v)
		
		if (u != v) {
			unio(comp, u, v)
			n--;
		}
	}
	
	cout << n << endl;
	
	for (int i = 0; i < n; i++) {
		cout << i << " " << find(comp, i) << endl;
	}
	
	return 0;
}

// Returns the set of that element
inline int find(vector<int> comp, int find) {
	int last = -1;
	while (comp[find] != find) {
		if (last != -1)
			comp[last] = comp[find]
		last = find
		find = comp[find]
	}
}

// Unions two sets
inline void unio(vector<int> comp, int u, int v) {
	if (u > v)
		comp[u] = v
	else
		comp[v] = u
}




















