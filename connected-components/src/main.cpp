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
#include "pBoost.cpp"
#include "SerialUnionFind.h"
#include "OpenMPCC.cpp"
#include "SpanningTreeCC.cpp"
#include "SerialRandomizedContractingCC.cpp"
#include "OpenMPRandomizedContractingCC.cpp"

#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
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

int main(int argc, char* argv[]) {
	// Argument parsing
	// ./a.exe bfs -g "graphs/g03.txt" -p 42
	// all arguments are optional
	int alg = 0;
	string fileName = "graphs/graph01.txt";
	for (int i = 1; i < argc; i++) {
		if (argv[i] == "bfs")
			alg = 0;
		else if (argv[i] == "ufind")
			alg = 1;
		else if (argv[i] == "contract")
			alg = 2;
		else if (argv[i] == "boost")
			alg = 3;
		else if (argv[i] == "pboost")
			alg = 4;
		else if (argv[i] == "pbfs")
			alg = 5;
		else if (argv[i] == "pstree")
			alg = 6;
		else if (argv[i] == "pcontract")
			alg = 7;
		else if (argv[i] == "-g") {// graph selection
			if (++i < argc)
				fileName = argv[i];
		}
		else if (argv[i] == "-p") {// # threads selection
			/*if (++i < argc) {
				string a = argv[i];
				bool isNumber = true;
				for(string::const_iterator k = a.begin(); k != a.end(); ++k)
				    isNumber &&= isdigit(*k);
				if (isNumber)
					omp_set_num_threads(stoi(argv[i]));
			}*/
		}
	}

	//ios_base::sync_with_stdio(false);

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
    if (alg == 0) {// bfs
    	SerialConnectedComponents cc;
    	cc.run(vertexCount, edges, vertexToComponent);
    } else if (alg == 1) {// ufind
    	SerialUnionFind cc;
    	cc.run(vertexCount, edges, vertexToComponent);
    } else if (alg == 2) {// contract
    	SerialRandomizedContractingCC cc;
    	cc.run(vertexCount, edges, vertexToComponent);
    } else if (alg == 3) {// boost
    	BoostCC cc;
    	cc.run(vertexCount, edges, vertexToComponent);
    } else if (alg == 4) {// pboost
    	pBoost cc;
    	cc.run(vertexCount, edges, vertexToComponent);
    } else if (alg == 5) {// pbfs
    	OpenMPCC cc;
    	cc.run(vertexCount, edges, vertexToComponent);
    } else if (alg == 6) {// pstree
    	SpanningTreeCC cc;
    	cc.run(vertexCount, edges, vertexToComponent);
    } else if (alg == 7) {// pcontract
    	OpenMPRandomizedContractingCC cc;
    	cc.run(vertexCount, edges, vertexToComponent);
    }

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
