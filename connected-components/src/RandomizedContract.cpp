
#include <vector>
#include <stack>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <algorithm>
#include <ctime>
#include <chrono>

using namespace std;

class RandomizedContract {

	public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &L) {
	    std::chrono::time_point<std::chrono::system_clock> start, end;
	    std::chrono::duration<double> elapsed_seconds;
	    start = std::chrono::system_clock::now();
		const int HEAD = 0;
		const int TAIL = 1;

		vector<int> headOrTail = vector<int>(numberOfVertices, -1); // head or tail
		vector<int> headOrTailIteration = vector<int>(numberOfVertices, -1); // head or tail
		vector<pair<int,int> > E;
		for (unsigned int i = 0; i < edges.size(); ++i) E.push_back(pair<int, int>(edges[i]));
		for (int i = 0; i < numberOfVertices; ++i) L[i] = i;
		int edgesLeft = edges.size();
		#ifdef __linux__
			unsigned int seed = 0;
		#endif
		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoint 1: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();

		stack<pair<int,int> > s;
		for (int iteration = 0; edgesLeft > 0; ++iteration) {
			for (int i = 0; i < edgesLeft; ++i) {
				int u = E[i].first;
				int v = E[i].second;
				if (u == v) continue;
				#ifdef __linux__
					if (headOrTailIteration[u] != iteration) {
						headOrTail[u] = rand_r(&seed)%2;
						headOrTailIteration[u] = iteration;
					}
					if (headOrTailIteration[v] != iteration) {
						headOrTail[v] = rand_r(&seed)%2;
						headOrTailIteration[v] = iteration;
					}
				#elif _WIN32
					if (headOrTailIteration[u] != iteration) {
						headOrTail[u] = rand()%2;
						headOrTailIteration[u] = iteration;
					}
					if (headOrTailIteration[v] != iteration) {
						headOrTail[v] = rand()%2;
						headOrTailIteration[v] = iteration;
					}
				#endif

				// in the parallel version, this part is separated in another for loop
				// if that is done in the serial version, the code becomes slower.
				// Probably because, if it is in the same loop, values of headOrTail[u]
				// are cached
				if (headOrTail[u] == TAIL && headOrTail[v] == HEAD) {
					L[u] = v;
//					cout << u << " joins " << v << endl;
				} else if (headOrTail[u] == HEAD && headOrTail[v] == TAIL) {
					L[v] = u;
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
					if (headOrTail[u] == TAIL && headOrTail[v] == HEAD) {
						s.push(pair<int,int>(v,u));
					} else if (headOrTail[u] == HEAD && headOrTail[v] == TAIL) {
						s.push(pair<int,int>(u,v));
					}

				}
			}
			edgesLeft = nonContractedEdges;
//			cout << "L - ";
//			for (int i = 0; i < numberOfVertices; ++i) cout << i << ":" << L[i] << ", ";
//			cout << endl << "iterate " << endl;
		}
		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoin 2: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();
		while (!s.empty()) {
			pair<int,int> e = s.top();
			s.pop();
			L[e.second] = L[e.first];
		}

		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Last checkpoint: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();

		return 0;
	}
};


