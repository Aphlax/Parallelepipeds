//============================================================================
// Name        : connected-components.cpp
// Author      : Seraiah, Gustavo, Fabian
// Version     : 0.0.0.1
// Copyright   : NONE
// Description : Connected Components in Parallel
//============================================================================

#include "RandomGraph.h"
#include "Bfs.cpp"
#include "Boost.cpp"
#include "pBoost.cpp"
#include "SerialUnionFind.h"
#include "OpenMPCC.cpp"
#include "SpanningTreeCC.cpp"
#include "RandomizedContract.cpp"
#include "PRandomizedContract.cpp"
#include "pBfsAtomic.cpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <stdexcept>   // for exception, runtime_error, out_of_range
#include <unordered_map>
#include <omp.h>

#include <emmintrin.h>

using namespace std;

void generateAndSaveGraph(const string& fileName) {
	ofstream myfile(fileName.c_str());

	if (!myfile.is_open()) throw std::runtime_error ("Unable to open file");

	int connectivity = 10;
	vector<int> sizeOfEachComponent;
	for (int i = 0; i < 10; ++i) {
		sizeOfEachComponent.push_back(100000 * (i + 1));
	}
	RandomGraph r(sizeOfEachComponent, connectivity);

	myfile << r.toString();
	myfile.close();

	cout << "Graph saved at: " << fileName << "\n";
}

vector<pair<int, int> > generateGraph(const int size, int* n, int* solution) {
	vector<int> cs(3, 1);
	cs.push_back(2);
	cs.push_back(2);
	cs.push_back(2);
	cs.push_back(3);
	cs.push_back(3);
	cs.push_back(5);
	cs.push_back(10);
	cs.push_back(20);
	cs.push_back(50);
	for (int i = 0; i < size; ++i) {
		cs.push_back(1000);
	}
	*solution = cs.size();
	*n = 100 + size * 1000;

	RandomGraph rg(cs, 10);

	return rg.getEdgeList();
}

/**
 * Returns number of vertices. Fills edges vector.
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

bool runAlgo(int alg, int vertexCount, vector<pair<int, int> > &edges, vector<int> &vertexToComponent, int solution, double *time) {
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	int nrComponents = 0;

	//----------------------------------------------
	//---------------Implementation-----------------
	//----------------------------------------------
	cout << "Running ";
	if (alg == 0) {// bfs
		cout << "bfs\n";
		Bfs cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	} else if (alg == 1) {// ufind
		cout << "ufind\n";
		SerialUnionFind cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	} else if (alg == 2) {// randcontract
		cout << "randcontract\n";
		RandomizedContract cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	} else if (alg == 3) {// boost
		cout << "boost\n";
		Boost cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	} else if (alg == 4) {// pboost
		cout << "pboost\n";
		pBoost cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	} else if (alg == 5) {// pbfs
		cout << "pbfs\n";
		OpenMPCC cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	} else if (alg == 6) {// pstree
		cout << "pstree\n";
		SpanningTreeCC cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	} else if (alg == 7) {// pcontract
		cout << "pbfsatomic\n";
		PRandomizedContract cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	} else if (alg == 8) {// pbfsatomic
		cout << "pbfsatomic\n";
		PBfsAtomic cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent);
	}

	//----------------------------------------------

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;

	*time = elapsed_seconds.count();

	return nrComponents == solution;
}

int main(int argc, char* argv[]) {

	#ifdef __linux__
		cout << "Running on linux\n";
	#elif _WIN32
		cout << "Not running on linux\n";
	#endif
	// Argument parsing
	// ./a.exe bfs -g "graphs/g03.txt" -p 42
	// all arguments are optional
	int alg = 0;
	string fileName = "graphs/graph01.txt";
	vector<int> testG(0);
	vector<int> testP(0);
	int repetitions = 1;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "bfs"))
			alg = 0;
		else if (!strcmp(argv[i], "ufind"))
			alg = 1;
		else if (!strcmp(argv[i], "randcontract"))
			alg = 2;
		else if (!strcmp(argv[i], "boost"))
			alg = 3;
		else if (!strcmp(argv[i], "pboost"))
			alg = 4;
		else if (!strcmp(argv[i], "pbfs"))
			alg = 5;
		else if (!strcmp(argv[i], "pstree"))
			alg = 6;
		else if (!strcmp(argv[i], "prandcontract"))
			alg = 7;
		else if (!strcmp(argv[i], "pbfsatomic"))
			alg = 8;
		else if (!strcmp(argv[i], "-g")) {// graph selection
			if (++i < argc)
				fileName = argv[i];
		}
		else if (!strcmp(argv[i], "-p")) {// # threads selection
			if (++i < argc) {
				std::istringstream iss(argv[i]);
				int p;
				if (iss >> p)
				{
					cout << "Set preffered number of threads to " << p << endl;
					omp_set_num_threads(p);
				}
			}
		}
		else if (!strcmp(argv[i], "-h")) {
			cout << "Options:\n\t[String]\tAlgorithm selection\n\t-g [String]\tPath to graph file\n\t-p [Int]\tNumber of threads hint\n\t-h\t\tThis Message\n\t-tg [Int]*\tTest performance with graphs of given sizes (*1000)\n\t-tp [Int]*\tTest performance with given number of threads\n\t-r [Int]\tRepetitions for tests\n";
			cout << "Algorithms: bfs, ufind, contract, boost\nParallel Algorithms: pboost, pbfs, pstree, pcontract" << endl;
			return 0;
		}
		else if (!strcmp(argv[i], "-tg")) {// testing graphs
			while (++i < argc)
			{
				std::istringstream iss(argv[i]);
				int g;
				if (iss >> g)
					testG.push_back(g);
				else {
					--i;
					break;
				}
			}
		}
		else if (!strcmp(argv[i], "-tp")) {// testing threads
			while (++i < argc)
			{
				std::istringstream iss(argv[i]);
				int p;
				if (iss >> p)
					testP.push_back(p);
				else {
					--i;
					break;
				}
			}
		}
		else if (!strcmp(argv[i], "-r")) {// repetitions for tests
			if (++i < argc) {
				std::istringstream iss(argv[i]);
				int r;
				if (iss >> r)
				{
					repetitions = r;
				}
			}
		}
		else {
			cout << "Invalid algorithm: " << argv[i] << endl;
			return 0;
		}
	}

	vector<pair<int,int> > edges;
	int vertexCount = 0;
	int nrComponents = 0;

	if (testG.size() == 0 && testP.size() == 0) {
		cout << "Max number of openMP threads: " << omp_get_max_threads() << endl;
		double time = 0;
		vertexCount = readGraphFile(fileName, edges);
		std::vector<int> vertexToComponent(vertexCount, -1);

		runAlgo(alg, vertexCount, edges, vertexToComponent, 0, &time);

	//	std::vector<int> sizeOfComponent;
	//	int componentCount = findSizeOfComponent(vertexCount, vertexToComponent, sizeOfComponent);
	//	for (int i = 0; i < componentCount; ++i) {
	//		cout << "Component " << i << ": " << sizeOfComponent[i] << " vertices\n";
	//	}

		cout << "Time elapsed: " << time << "s\n";
	} else if (testG.size() != 0) {
		int sol = 0;
		int ok = 0;
		vector<double> time(testG.size());
		for (int i = 0; i < testG.size(); i++) {
			edges = generateGraph(testG[i], &vertexCount, &sol);
			double t = 0;
			for (int j = 0; j < repetitions; j++) {
				std::vector<int> vertexToComponent(vertexCount, -1);
				bool result = runAlgo(alg, vertexCount, edges, vertexToComponent, sol, &t);
				ok += result ? 0 : 1;
				time[i] += t;
			}
			time[i] = time[i] / repetitions;
		}
		if (ok == 0)
			cout << "Test successful!" << endl;
		else
			cout << "Errors occured " << ok << " times!" << endl;
		cout << "Timings:\n";
		for (int i = 0; i < testG.size(); i++) {
			cout << "\t" << time[i];
		}
		cout << endl;
	} else { // testP nonempty
		int sol = 10;
		vector<double> time(testP.size());
		vertexCount = readGraphFile(fileName, edges);
		for (int i = 0; i < testP.size(); i++) {
			omp_set_num_threads(testP[i]);
			double t = 0;
			for (int j = 0; j < repetitions; j++) {
				std::vector<int> vertexToComponent(vertexCount, -1);
				bool result = runAlgo(alg, vertexCount, edges, vertexToComponent, sol, &t);
				time[i] += t;
			}
			time[i] = time[i] / repetitions;
		}
		cout << "Timings:\n";
		for (int i = 0; i < testP.size(); i++) {
			cout << "\t" << time[i];
		}
		cout << endl;
	}

	return 0;
}
