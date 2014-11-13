// g++ main.cpp BoostCC.cpp RandomGraph.cpp SerialConnectedComponents.cpp openMPCC.cpp -fopenmp


#include <vector>
#include <stack>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <algorithm>

using namespace std;

class SerialRandomizedContractingCC {

	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &L) {
		const int HEAD = 0;
		const int TAIL = 1;

		cout << "SerialRandomizedContractingMPCC started" << endl;
		vector<pair<int,int> > headOrTail = vector<pair<int, int> >(numberOfVertices, pair<int, int>(0, -1)); // head or tail, iteration
		vector<pair<int,int> > E;
		for (unsigned int i = 0; i < edges.size(); ++i) E.push_back(pair<int, int>(edges[i]));
		for (int i = 0; i < numberOfVertices; ++i) L[i] = i;
		int edgesLeft = edges.size();
		unsigned int seed = 0;

		stack<pair<int,int> > s;
		for (int iteration = 0; edgesLeft > 0; ++iteration) {
			for (int i = 0; i < edgesLeft; ++i) {
				int u = E[i].first;
				int v = E[i].second;
				if (u == v) continue;
				if (headOrTail[u].second != iteration) {
//					headOrTail[u] = pair<int,int>(rand_r(&seed)%2, iteration);
					headOrTail[u] = pair<int,int>(rand()%2, iteration);
				}
				if (headOrTail[v].second != iteration) {
					//headOrTail[v] = pair<int,int>(rand_r(&seed)%2, iteration);
					headOrTail[v] = pair<int,int>(rand()%2, iteration);
				}

				if (headOrTail[u].first == TAIL && headOrTail[v].first == HEAD) {
					L[u] = v;
//					cout << u << " joins " << v << endl;
//				} else if (headOrTail[u].first == HEAD && headOrTail[v].first == TAIL) {
//					L[v] = u;
//					cout << v << " joins " << u << endl;
				}
			}
			int nonContractedEdges = 0;
			for (int i = 0; i < edgesLeft; ++i) {
				int u = E[i].first;
				int v = E[i].second;
				if (L[u] != L[v]) {
//					cout << "Adding edge: " << L[u] << ", " << L[v]  << " from " << u << ", " << v  << endl;
					E[nonContractedEdges].first = L[u];
					E[nonContractedEdges].second = L[v];
					++nonContractedEdges;
				} else {
//					cout << u << " and " << v << " are merged to " << L[v]  << endl;
					if (headOrTail[u].first == TAIL && headOrTail[v].first == HEAD) {
						s.push(pair<int,int>(v,u));
//					} else if (headOrTail[u].first == HEAD && headOrTail[v].first == TAIL) {
//						s.insert(pair<int,int>(u,v));
					}

				}
			}
			edgesLeft = nonContractedEdges;
//			cout << "L - ";
//			for (int i = 0; i < numberOfVertices; ++i) cout << i << ":" << L[i] << ", ";
//			cout << endl << "iterate " << endl;
		}
		while (!s.empty()) {
			pair<int,int> e = s.top();
			s.pop();
			L[e.second] = L[e.first];
		}

		return 0;
	}
};


