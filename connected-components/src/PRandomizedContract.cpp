
#include <vector>
#include <stack>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <algorithm>
#include "StopWatch.cpp"
#include <atomic>

using namespace std;

class PRandomizedContract {

public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &L) {
	StopWatch stopWatch;
	stopWatch.start(stopWatch.inputProcessing);
	const int HEAD = 0;
	const int TAIL = 1;

	vector<int> headOrTail = vector<int>(numberOfVertices, -1); // head or tail
	vector<int> headOrTailIteration = vector<int>(numberOfVertices, -1); // head or tail

	vector<pair<int,int> > E = vector<pair<int,int> >(edges.size(), pair<int, int>(-1,-1));

	#pragma omp parallel for default(none) shared(E, edges)
	for (unsigned int i = 0; i < edges.size(); ++i) E[i] = pair<int, int>(edges[i]);

	#pragma omp parallel for default(none) shared(L)
	for (int i = 0; i < numberOfVertices; ++i) L[i] = i;


	int edgesLeft = edges.size();

	vector<unsigned int> seeds = vector<unsigned int>(omp_get_max_threads());
	stack<pair<int,int> > s;
	vector<vector<pair<int,int> > > contractedEdges = vector<vector<pair<int,int> > >(omp_get_max_threads(), vector<pair<int,int> >(0));
	vector<vector<int> > contractedEdgesInIteration = vector<vector<int> >(omp_get_max_threads(), vector<int>(0));
	for (int i = 0; i < omp_get_max_threads(); ++i) seeds[i] = i;

	cout << "Checkpoint 1: " << stopWatch.stop(stopWatch.inputProcessing) << "s\n";
	stopWatch.start(stopWatch.mainSection);

	for (int iteration = 0; edgesLeft > 0; ++iteration) {

		// some headOrTail values might be recomputed, but that is ok, they just need to be consistent after the end of the for loop
		#pragma omp parallel for default(none) shared(edgesLeft, E, headOrTail, headOrTailIteration, seeds, iteration)
		for (int i = 0; i < edgesLeft; ++i) {
			int u = E[i].first;
			int v = E[i].second;
			if (u == v) continue;
			#ifdef __linux__
				if (headOrTailIteration[u] != iteration) {
					headOrTail[u] = rand_r(&seeds[omp_get_thread_num()])%2;
					headOrTailIteration[u] = iteration;
				}
				if (headOrTailIteration[v] != iteration) {
					headOrTail[v] = rand_r(&seeds[omp_get_thread_num()])%2;
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
		}

		#pragma omp parallel for default(none) shared(edgesLeft, E, headOrTail, L)
		for (int i = 0; i < edgesLeft; ++i) {
			int u = E[i].first;
			int v = E[i].second;
			if (u == v) continue;
			if (headOrTail[u] == TAIL && headOrTail[v] == HEAD) {
				L[u] = v;
			} else if (headOrTail[u] == HEAD && headOrTail[v] == TAIL) {
				L[v] = u;
			}
		}

////		OPTION 1 --------------------------------------------------------------------------------------------------------------------------------------
//		std::atomic<int> nonContractedEdgeCount(0);
//		#pragma omp parallel for default(none) shared(edgesLeft, E, headOrTail, L, contractedEdges, contractedEdgesInIteration, nonContractedEdgeCount)
//		for (int i = 0; i < edgesLeft; ++i) {
//			int u = E[i].first;
//			int v = E[i].second;
//			if (L[u] != L[v]) {
//				int curCount = nonContractedEdgeCount.fetch_add(1);
//				E[curCount].first = L[u];
//				E[curCount].second = L[v];
//			}
//		}
//		edgesLeft = nonContractedEdgeCount;
//
//		for (int i = 0; i < edgesLeft; ++i) {
//			int u = E[i].first;
//			int v = E[i].second;
//			if (L[u] == L[v]) {
//				if (headOrTail[u] == TAIL && headOrTail[v] == HEAD) {
//					s.push(pair<int,int>(v,u));
//				} else if (headOrTail[u] == HEAD && headOrTail[v] == TAIL) {
//					s.push(pair<int,int>(u,v));
//				}
//			}
//		}

////		OPTION 2 --------------------------------------------------------------------------------------------------------------------------------------
//		#pragma omp parallel default(none) shared(edgesLeft, E, headOrTail, L, contractedEdges, contractedEdgesInIteration)
//		{
//			contractedEdgesInIteration[omp_get_thread_num()].push_back(0);
//			#pragma omp for nowait
//			for (int i = 0; i < edgesLeft; ++i) {
//				int u = E[i].first;
//				int v = E[i].second;
//				if (L[u] == L[v]) {
//					if (headOrTail[u] == TAIL && headOrTail[v] == HEAD) {
//						contractedEdges[omp_get_thread_num()].push_back(pair<int,int>(v,u));
//						++contractedEdgesInIteration[omp_get_thread_num()].back();
//					} else if (headOrTail[u] == HEAD && headOrTail[v] == TAIL) {
//						contractedEdges[omp_get_thread_num()].push_back(pair<int,int>(u,v));
//						++contractedEdgesInIteration[omp_get_thread_num()].back();
//					}
//
//				}
//			}
//		}
//		int nonContractedEdgeCount = 0;
//		for (int i = 0; i < edgesLeft; ++i) {
//			int u = E[i].first;
//			int v = E[i].second;
//			if (L[u] != L[v]) {
//				int curCount = nonContractedEdgeCount;
//				E[curCount].first = L[u];
//				E[curCount].second = L[v];
//				++nonContractedEdgeCount;
//			}
//		}
//		edgesLeft = nonContractedEdgeCount;


//		OPTION 3 --------------------------------------------------------------------------------------------------------------------------------------
		int nonContractedEdgeCount = 0;
		for (int i = 0; i < edgesLeft; ++i) {
			int u = E[i].first;
			int v = E[i].second;
			if (L[u] != L[v]) {
				int curCount = nonContractedEdgeCount;
				E[curCount].first = L[u];
				E[curCount].second = L[v];
				++nonContractedEdgeCount;
			} else {
				if (headOrTail[u] == TAIL && headOrTail[v] == HEAD) {
					s.push(pair<int,int>(v,u));
				} else if (headOrTail[u] == HEAD && headOrTail[v] == TAIL) {
					s.push(pair<int,int>(u,v));
				}
			}
		}
		edgesLeft = nonContractedEdgeCount;

////		OPTION 4 --------------------------------------------------------------------------------------------------------------------------------------
//		std::atomic<int> nonContractedEdgeCount(0);
//		#pragma omp parallel default(none) shared(edgesLeft, E, headOrTail, L, nonContractedEdgeCount, contractedEdges, contractedEdgesInIteration)
//		{
//			contractedEdgesInIteration[omp_get_thread_num()].push_back(0);
//			#pragma omp for
//			for (int i = 0; i < edgesLeft; ++i) {
//				int u = E[i].first;
//				int v = E[i].second;
//				if (L[u] != L[v]) {
//					int curCount = nonContractedEdgeCount.fetch_add(1);
//					E[curCount].first = L[u];
//					E[curCount].second = L[v];
//				} else {
//					if (headOrTail[u] == TAIL && headOrTail[v] == HEAD) {
//						contractedEdges[omp_get_thread_num()].push_back(pair<int,int>(v,u));
//						++contractedEdgesInIteration[omp_get_thread_num()].back();
//					} else if (headOrTail[u] == HEAD && headOrTail[v] == TAIL) {
//						contractedEdges[omp_get_thread_num()].push_back(pair<int,int>(u,v));
//						++contractedEdgesInIteration[omp_get_thread_num()].back();
//					}
//
//				}
//			}
//		}
//		edgesLeft = nonContractedEdgeCount;


//			cout << "L - ";
//			for (int i = 0; i < numberOfVertices; ++i) cout << i << ":" << L[i] << ", ";
//			cout << endl << "iterate " << endl;

	}

	vector<int> contractedEdgeCounter = vector<int>(omp_get_max_threads(), 0);
	for (int i = contractedEdgesInIteration.size() - 1; i >= 0; --i) {

	}

//	while (!s.empty()) {
//		pair<int,int> e = s.top();
//		s.pop();
//		L[e.second] = L[e.first];
//	}

	cout << "Last checkpoint: " << stopWatch.stop(stopWatch.mainSection) << "s\n";

	return 0;
}
};


