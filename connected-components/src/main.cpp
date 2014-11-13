//============================================================================
// Name        : connected-components.cpp
// Author      : Seraiah, Gustavo, Fabian
// Version     : 0.0.0.1
// Copyright   : NONE
// Description : Connected Components in Parallel
//============================================================================

#include "RandomGraph.h"
#include "SerialConnectedComponents.h"
#include "BoostCC.h"
#include "OpenMPCC.cpp"
#include "SerialRandomizedContractingCC.cpp"
#include "OpenMPRandomizedContractingCC.cpp"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <stdexcept>   // for exception, runtime_error, out_of_range
#include <unordered_map>
using namespace std;

void generateAndSaveGraph(const string& fileName) {
	ofstream myfile(fileName.c_str());

	if (!myfile.is_open()) throw std::runtime_error ("Unable to open file");

	int connectivity = 10;
	vector<int> sizeOfEachComponent;
	for (int i = 0; i < 10; ++i) {
		sizeOfEachComponent.push_back(100 * (i + 1));
	}
	RandomGraph r(sizeOfEachComponent, connectivity);

	myfile << r.toString();
	myfile.close();

	cout << "Graph saved at: " << fileName << "\n";
}

/**
 * Returns number of vertices. Filles edges vector.
 */
int readGraphFile(const string& fileName, vector<pair<int, int> > &outEdges) {
	ifstream myfile(fileName.c_str());
	if (!myfile.is_open()) throw std::runtime_error ("Unable to open file");

	int vectorCount = -1;
	string line;
	while (getline(myfile, line)) {
		size_t pos = line.find(" ");
		int firstInt = atoi(line.substr(0, pos).c_str());
		if (vectorCount == -1) {
			vectorCount = firstInt;
			continue;
		}
		int secondInt = atoi(line.substr(pos + 1).c_str());
		outEdges.push_back(pair<int, int>(firstInt, secondInt));
	}
	myfile.close();
	cout << "Graph at: " << fileName << " was read.\n";
	return vectorCount;
}


int findSizeOfComponent(const int vertexCount, const std::vector<int>& vertexToComponent, std::vector<int>& outSizeOfComponent) {
	std::unordered_map<int, int> m;
	for(int i = 0; i < vertexCount; ++i) {
		std::unordered_map<int, int>::iterator it = m.find(vertexToComponent[i]);
		if (it == m.end()) {
			m.insert(pair<int,int>(vertexToComponent[i], 1));
		} else {
			++(it->second);
		}
	}
	outSizeOfComponent.resize(0);
	for (std::unordered_map<int, int>::iterator it = m.begin(); it != m.end(); ++it) {
		outSizeOfComponent.push_back(it->second);
	}
	sort(outSizeOfComponent.begin(), outSizeOfComponent.end());
	return outSizeOfComponent.size();
}

int main() {

//	ios_base::sync_with_stdio(false);

	string fileName = "graphs/graph01.txt";
//	uncomment to generate other graphs
//	generateAndSaveGraph(fileName);
	vector<pair<int,int> > edges;
	int vertexCount = readGraphFile(fileName, edges);
	std::vector<int> vertexToComponent(vertexCount, -1);

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

	//----------------------------------------------
	//---------------Implementation-----------------
	//----------------------------------------------

	//SerialConnectedComponents scc;
	BoostCC bcc;
	OpenMPCC ompcc;
	int componentCount = bcc.run(vertexCount, edges, vertexToComponent);
	SerialConnectedComponents cc;
	//BoostCC cc;
//    OpenMPCC cc;
//    SerialRandomizedContractingCC cc;
//    OpenMPRandomizedContractingCC cc;
    cc.run(vertexCount, edges, vertexToComponent);

	//----------------------------------------------

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;

	std::vector<int> sizeOfComponent;
	int componentCount = findSizeOfComponent(vertexCount, vertexToComponent, sizeOfComponent);
	for (int i = 0; i < componentCount; ++i) {
		cout << "Component " << i << ": " << sizeOfComponent[i] << " vertices\n";
	}

	cout << "Time elapsed: " << elapsed_seconds.count() << "s\n";

	return 0;
}
