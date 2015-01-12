
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

private:

const int baseLength = 10000;

public: int parallelPrefixSum(vector<int> &v, const int begin, const int end) {
		int length = end - begin;
		int mid = begin + length / 2;
		int sum = 0;

		if (length <= baseLength) {
			for (int ii = begin + 1; ii < end; ii++) {
				v[ii] += v[ii - 1];
			}
		} else {
			#pragma omp task shared(sum, v)
			{
				sum = parallelPrefixSum(v, begin, mid);
			}
			#pragma omp task shared(v)
			{
				parallelPrefixSum(v, mid, end);
			}
			#pragma omp taskwait

			#pragma omp parallel for
			for (int ii = mid; ii < end; ii++) {
				v[ii] += sum;
			}

		}
		return v[end - 1];
	}

public: int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &L, StopWatch &stopWatch) {
	stopWatch.start(stopWatch.inputProcessing);
	const int HEAD = 0;
	const int TAIL = 1;

	vector<int> headOrTail = vector<int>(numberOfVertices, -1); // head or tail
	vector<int> headOrTailIteration = vector<int>(numberOfVertices, -1); // head or tail

	vector<int> S = vector<int>(edges.size(), -1); // head or tail
	vector<pair<int,int> > E = vector<pair<int,int> >(edges.size(), pair<int, int>(-1,-1));
	vector<pair<int,int> > E_tmp = vector<pair<int,int> >(edges.size(), pair<int, int>(-1,-1));

	#pragma omp parallel for default(none) shared(E, edges)
	for (unsigned int i = 0; i < edges.size(); ++i) E[i] = pair<int, int>(edges[i]);

	#pragma omp parallel for default(none) shared(L)
	for (int i = 0; i < numberOfVertices; ++i) L[i] = i;


	int edgesLeft = edges.size();

	vector<unsigned int> seeds = vector<unsigned int>(omp_get_max_threads());
	stack<vector<vector<pair<int,int> > > > s;
	for (int i = 0; i < omp_get_max_threads(); ++i) seeds[i] = i;

	stopWatch.stop(stopWatch.inputProcessing);
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
			#else
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

		vector<vector<pair<int,int> > > aux(omp_get_max_threads(), vector<pair<int,int> >());

		#pragma omp parallel for default(none) shared(edgesLeft, E, E_tmp, headOrTail, L, S, s, aux)
		for (int i = 0; i < edgesLeft; ++i) {
			E_tmp[i] = E[i];
			int u = E[i].first;
			int v = E[i].second;
			if (L[u] == L[v]) {
				if (headOrTail[u] == TAIL && headOrTail[v] == HEAD) {
					aux[omp_get_thread_num()].push_back(pair<int,int>(v,u));
				} else if (headOrTail[u] == HEAD && headOrTail[v] == TAIL) {
					aux[omp_get_thread_num()].push_back(pair<int,int>(u,v));
				}
				S[i] = 0;
			} else {
				S[i] = 1;
			}
		}

		s.push(aux);

//		parallelPrefixSum(S, 0, edgesLeft); // << serial is faster than parallel
		for (int i = 1; i < edgesLeft; ++i) {
			S[i] += S[i-1];
		}


		#pragma omp parallel for default(none) shared(edgesLeft, E, E_tmp, L, S)
		for (int i = 0; i < edgesLeft; ++i) {
			int u = E_tmp[i].first;
			int v = E_tmp[i].second;
			if (L[u] != L[v]) {
				E[S[i]-1].first = L[u];
				E[S[i]-1].second = L[v];
			}
		}
		edgesLeft = S[edgesLeft-1];

	}

	stopWatch.stop(stopWatch.mainSection);
	stopWatch.start(stopWatch.merging);


	while (!s.empty()) {
		vector<vector<pair<int,int> > > e = s.top();
		s.pop();
		#pragma omp parallel default(none) shared(e, L)
		{
			int tn = omp_get_thread_num();
			for (unsigned int j = 0; j < e[tn].size(); j++) {
				L[e[tn][j].second] = L[e[tn][j].first];
			}
		}
	}

	stopWatch.stop(stopWatch.merging);
	return 0;
}
};


