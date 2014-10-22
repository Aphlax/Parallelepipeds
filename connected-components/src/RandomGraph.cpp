
#include "RandomGraph.h"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

void RandomGraph::print(vector<int> &v){
	if (v.size() == 0) {
		cout << "empty!\n";
	}
	for (unsigned int i = 0; i < v.size(); ++i) {
		cout << v[i] << "\n";
	}
}

RandomGraph::RandomGraph(const std::vector<int> &sizeOfEachComponent, const double averageConnectivity) {
	cout << "Generating graph!" << endl;

	srand((unsigned)time(NULL));

	int n = 0; //number of vertices
	for (unsigned int i = 0; i < sizeOfEachComponent.size(); ++i) n += sizeOfEachComponent[i];
	vertexToVertices.resize(n, set<int>());


	int nextIndex = 0;
	edgeCount = 0;
	for (unsigned int i = 0; i < sizeOfEachComponent.size(); ++i) {
		const int cn = sizeOfEachComponent[i]; // component size
		int componentStart = nextIndex; // inclusive
		vector<int> connected;
		vector<int> notConnected;
		for (int j = 0; j < cn; ++j) {
			if (j == 0) {
				connected.push_back(nextIndex);
			} else {
				notConnected.push_back(nextIndex);
			}
			++nextIndex;
		}

		// first make sure everyone is connected
		for (unsigned int j = 0; j < notConnected.size(); ++j) {
			int c = notConnected[j]; //current vertex
			int r = rand() % connected.size() + componentStart; // random vertex
			connect(vertexToVertices, c, r);
			connected.push_back(c);
		}

		int componentEdgeCount = notConnected.size();
		int targetEdgeCount = min((int) (averageConnectivity * cn),	(cn * (cn - 1)) / 2);

		while (componentEdgeCount < targetEdgeCount) {
			int r1 = rand() % cn + componentStart; // random vertex
			int r2 = rand() % cn + componentStart; // random vertex
			if (connect(vertexToVertices, r1, r2)) ++componentEdgeCount;
		}
		edgeCount += componentEdgeCount;
	}

	randomize();
}

void RandomGraph::randomize() {
	vector<set<int> > randomizedVertexToVertices(vertexToVertices.size(), set<int>());

	vector<int> shuffledIds;
	for (unsigned int i = 0; i < vertexToVertices.size(); ++i) shuffledIds.push_back(i);
	random_shuffle(shuffledIds.begin(), shuffledIds.end());
//	print(shuffledIds);

	for (unsigned int i = 0; i < vertexToVertices.size(); ++i) {
		for (set<int>::iterator it = vertexToVertices[i].begin(); it != vertexToVertices[i].end(); ++it) {
			connect(randomizedVertexToVertices, shuffledIds[i], shuffledIds[*it]);
		}
	}

	vertexToVertices = randomizedVertexToVertices;
}

// returns false if element already exists
bool RandomGraph::connect(vector<set<int> > &g, const int a, const int b) {
	int x = min(a,b);
	int y = max(a,b);
	return g[x].insert(y).second;
}


int RandomGraph::getEdgeCount() {
	return edgeCount;
}

std::string RandomGraph::toString() {
	ostringstream os;
	os << vertexToVertices.size() << " " << edgeCount << "\n";
	for (unsigned int i = 0; i < vertexToVertices.size(); ++i) {
	  for (set<int>::iterator it = vertexToVertices[i].begin(); it != vertexToVertices[i].end(); ++it) {
		  os << i << " " << *it << "\n";
	  }
	}
	return os.str();
}

std::vector<std::set<int> >* RandomGraph::getGraphDatastructure() {
	return &vertexToVertices;
}

