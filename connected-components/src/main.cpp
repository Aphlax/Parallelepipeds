//============================================================================
// Name        : connected-components.cpp
// Author      : Seraiah, Gustavo, Fabian
// Version     : 0.0.0.1
// Copyright   : NONE
// Description : Connected Components in Parallel
//============================================================================

//#define USE_MPI

#include "RandomGraph.h"
#include "Bfs.cpp"
#include "Boost.cpp"
#include "PBoost.h"
#include "SerialUnionFind.h"
#include "OpenMPCC.cpp"
#include "SpanningTreeCC.cpp"
#include "RandomizedContract.cpp"
#include "PRandomizedContract.cpp"
#include "pBfsAtomic.cpp"
#include "ObjConverter.cpp"
#include "StopWatch.cpp"

#include <iostream>
#include <fstream>
#include <sstream>
//#include <iomanip>
#include <string>
#include <ctype.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <stdexcept>   // for exception, runtime_error, out_of_range
#include <unordered_map>
#include <omp.h>
#ifdef USE_MPI
#include <mpi.h>
#endif

#include <emmintrin.h>


using namespace std;

int mpiThreadCount;
int mpiProcessRank;

void generateAndSaveGraph(const string& fileName) {
	ofstream myfile(fileName.c_str());

	if (!myfile.is_open()) throw std::runtime_error ("Unable to open file");

	int connectivity = 10;
	vector<int> sizeOfEachComponent;
	for (int i = 0; i < 10; ++i) {
		sizeOfEachComponent.push_back(50000 * (i + 1));
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
	cout << "Graph at: " << fileName << " was read. Graph has "<< vectorCount << " vertices and " << outEdges.size() << " edges.\n";
	return vectorCount;
}

/**
 * Returns the arguments found in input.txt.
 */
void readArguments(int* argc, vector<string> &args) {
	//cout << "reading arguments from input.txt" << endl;
	ifstream myfile("input.txt");
	if (!myfile.is_open()) return;

	int i = 0;
	string line, sub;
	if (getline(myfile, line)) {
		std::istringstream iss(line);
		do
		{
			iss >> sub;
			args[i++] = sub;
		} while (iss && i < 100);
	}
	myfile.close();
	*argc = i - 1;
	return;
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

void printTimings(StopWatch &stopWatch)
{
	cout << "Input processing: " << stopWatch.inputProcessingTime << endl;
	cout << "Main section:     " << stopWatch.mainSectionTime << endl;
	cout << "Merge section:    " << stopWatch.mergingTime << endl;
}

bool runAlgo(int alg, int vertexCount, vector<pair<int, int> > &edges, vector<int> &vertexToComponent, int solution, StopWatch &stopWatch) {
	int nrComponents = 0;

	//----------------------------------------------
	//---------------Implementation-----------------
	//----------------------------------------------
	if (mpiProcessRank == 0) cout << "Running ";
	if (alg == 0) {// bfs
		cout << "bfs\n";
		Bfs cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
	} else if (alg == 1) {// ufind
		cout << "ufind\n";
		SerialUnionFind cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
	} else if (alg == 2) {// randcontract
		cout << "randcontract\n";
		RandomizedContract cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
	} else if (alg == 3) {// boost
		cout << "boost\n";
		Boost cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
	} else if (alg == 4) {// pboost
#ifdef USE_MPI
		if (mpiProcessRank == 0) cout << "pboost\n";
		PBoost cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
#endif
	} else if (alg == 5) {// pbfs
		cout << "pbfs\n";
		OpenMPCC cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
	} else if (alg == 6) {// pstree
		cout << "pstree\n";
		SpanningTreeCC cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
	} else if (alg == 7) {// pcontract
		cout << "prandcontract\n";
		PRandomizedContract cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
	} else if (alg == 8) {// pbfsatomic
		cout << "pbfsatomic\n";
		PBfsAtomic cc;
		nrComponents = cc.run(vertexCount, edges, vertexToComponent, stopWatch);
	}

	//----------------------------------------------
	return nrComponents == solution;
}

//int main(int argc, char* argv[]) {
//	ObjConverter objToGraph;
//	objToGraph.convert("graphs/space_station.obj", "graphs/space_station.txt");
//	return 0;
//}

int main(int argc, char* arg[]) {
#ifdef USE_MPI
	MPI_Init(0,0);
#endif

	vector<string> argv(100);
	if (argc <= 1) {
		readArguments(&argc, argv);
	} else {
		for (int i = 1; i < argc; i++)// ignoring first argument: execution path
			argv[i - 1] = string(arg[i]);
		argc--;
	}

	// Argument parsing
	// ./a.exe bfs -g "graphs/g03.txt" -p 42
	// all arguments are optional
	int alg = 0;
	string fileName = "graphs/graph01.txt";
	bool testFiles = false;
	vector<int> testG(0);
	vector<int> testP(0);
	int repetitions = 1;
	for (int i = 0; i < argc; i++) {
		if (!argv[i].compare("bfs"))
			alg = 0;
		else if (!argv[i].compare("ufind"))
			alg = 1;
		else if (!argv[i].compare("randcontract"))
			alg = 2;
		else if (!argv[i].compare("boost"))
			alg = 3;
		else if (!argv[i].compare("pboost"))
			alg = 4;
		else if (!argv[i].compare("pbfs"))
			alg = 5;
		else if (!argv[i].compare("pstree"))
			alg = 6;
		else if (!argv[i].compare("prandcontract"))
			alg = 7;
		else if (!argv[i].compare("pbfsatomic"))
			alg = 8;
		else if (!argv[i].compare("-g")) {// graph selection
			if (++i < argc)
				fileName = argv[i];
		}
		else if (!argv[i].compare("-p")) {// # threads selection
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
		else if (!argv[i].compare("-h")) {
			cout << "Options:\n\t[String]\tAlgorithm selection\n\t-g [String]\tPath to graph file\n\t-p [Int]\tNumber of threads hint\n\t-h\t\tThis Message\n\t-tg [Int]*\tTest performance with graphs of given sizes (*1000)\n\t-tf [Int]*\tTest performance with multiple graph files. Requires -g option to be set and the name\n\t\t\tmust contain the character # (which is then replaced with the given integers as two digits)\n\t-tp [Int]*\tTest performance with given number of threads\n\t-r [Int]\tRepetitions for tests\n";
			cout << "Algorithms: bfs, ufind, randcontract, boost\nParallel Algorithms: pboost, pbfs, pbfsatomic, pstree, prandcontract" << endl;
			return 0;
		}
		else if (!argv[i].compare("-tg") || !argv[i].compare("-tf")) {// testing graphs w/wo files
			testFiles = !argv[i].compare("-tf");
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
		else if (!argv[i].compare("-tp")) {// testing threads
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
		else if (!argv[i].compare("-r")) {// repetitions for tests
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

#ifdef USE_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &mpiThreadCount);
	MPI_Comm_rank (MPI_COMM_WORLD, &mpiProcessRank);
#endif
	StopWatch stopWatch;


	vector<pair<int,int> > edges;
	int vertexCount = 0;

	if (testG.size() == 0 && testP.size() == 0) { // single run!
		if (mpiProcessRank == 0) {
			#ifdef __linux__
				cout << "Running on linux\n";
			#elif _WIN32
				cout << "Not running on linux\n";
			#endif

			cout << "Max number of openMP threads: " << omp_get_max_threads() << endl;
			cout << "Max number of MPI threads: " << mpiThreadCount << endl;
			vertexCount = readGraphFile(fileName, edges);

		}
		if (mpiThreadCount > 1) {
			// multiple processes. need to brodcast vertexCount
#ifdef USE_MPI
			MPI_Bcast(&vertexCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif
		}
		std::vector<int> vertexToComponent(vertexCount, -1);

		runAlgo(alg, vertexCount, edges, vertexToComponent, 0, stopWatch);

		if (mpiProcessRank == 0) {
			std::vector<int> sizeOfComponent;
			int componentCount = findSizeOfComponent(vertexCount, vertexToComponent, sizeOfComponent);
			for (int i = 0; i < componentCount; ++i) {
				cout << "Component " << i << ": " << sizeOfComponent[i] << " vertices\n";
			}
			printTimings(stopWatch);
		}

	} else if (testG.size() != 0) { // test with various sized graphs

		int sol = 0, pos = -1;
		vector<double> timeMain(testG.size());
		vector<double> timeMerge(testG.size());
		if (testFiles) {
			pos = fileName.find("#");
			if (pos == -1 || pos == ((int)fileName.length()) - 1) {
				cout << "You have to specify a filename with a # in it. (-tf requires -g)" << endl;
				return 0;
			}
		}
		for (unsigned int i = 0; i < testG.size(); i++) {
			if (mpiProcessRank == 0) {
				if (!testFiles) // generate graph
					edges = generateGraph(testG[i], &vertexCount, &sol);
				else {// load graph from file
					stringstream ss;
					//ss << setw(2) << setfill('0') << testG[i];
					if (testG[i] < 10)
						ss << 0;
					ss << testG[i];
					string fname = fileName.substr(0, pos) + ss.str() + fileName.substr(pos + 1);
					vertexCount = readGraphFile(fname, edges);
				}
			}
			if (mpiThreadCount > 1) {
				// multiple processes. need to brodcast vertexCount
#ifdef USE_MPI
				MPI_Bcast(&vertexCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif
				}

			for (int j = 0; j < repetitions; j++) {
				std::vector<int> vertexToComponent(vertexCount, -1);
				runAlgo(alg, vertexCount, edges, vertexToComponent, sol, stopWatch);
				if (mpiProcessRank == 0)
				{
					timeMain[i] += stopWatch.mainSectionTime;
					timeMerge[i] += stopWatch.mergingTime;
				}
			}
			if (mpiProcessRank == 0)
			{
				timeMain[i] = timeMain[i] / repetitions;
				timeMerge[i] = timeMerge[i] / repetitions;
			}
		}
		if (mpiProcessRank == 0)  {
			cout << "Timings: \n";
			cout << "\t Main:  \t Merge:\n";
			for (unsigned int i = 0; i < testG.size(); i++) {
				cout << "\t" << timeMain[i] << "\t" << timeMerge[i] << "\n";
			}
			cout << endl;
		}

	} else { // testP nonempty, test with different amounts of threads
		// this assumes non-mpi, no need to put mpi checks
		int sol = 10;
		vector<double> timeMain(testP.size());
		vector<double> timeMerge(testP.size());
		vertexCount = readGraphFile(fileName, edges);
		for (unsigned int i = 0; i < testP.size(); i++) {
			omp_set_num_threads(testP[i]);
			for (int j = 0; j < repetitions; j++) {
				std::vector<int> vertexToComponent(vertexCount, -1);
				runAlgo(alg, vertexCount, edges, vertexToComponent, sol, stopWatch);
				timeMain[i] += stopWatch.mainSectionTime;
				timeMerge[i] += stopWatch.mergingTime;
			}
			timeMain[i] = timeMain[i] / repetitions;
			timeMerge[i] = timeMerge[i] / repetitions;
		}
		cout << "Timings: \n";
		cout << "\t Main:  \t Merge:\n";
		for (unsigned int i = 0; i < testP.size(); i++) {
			cout << "\t" << timeMain[i] << "\t" << timeMerge[i] << "\n";
		}
		cout << endl;

	}


//#ifdef __linux__
#ifdef USE_MPI
	MPI_Barrier(MPI_COMM_WORLD);
	// for memory measurements
	ifstream statusFile("/proc/self/status");
	if (!statusFile.is_open()) throw std::runtime_error ("Unable to open status file for memory measurments");

	string line;
	while (getline(statusFile, line)) {
//		if (mpiProcessRank == 0) cout << line << endl;
		if (strncmp(line.c_str(), "VmHWM:", 6) == 0) {
			if (mpiProcessRank == 0)  {
				cout << "Process " << mpiProcessRank << " peak mem usage: " << line.substr(6) << endl;

				for (int i = 1; i < mpiThreadCount; ++i) {
					MPI_Status status;
					MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
					int l;
					MPI_Get_count(&status, MPI_CHAR, &l);
					char *buf = new char[l];
					MPI_Recv(buf, l, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
					cout << "Process " << i << " peak mem usage: " << buf << endl;
					delete [] buf;
				}
			} else {
				string msg = line.substr(6);
				MPI_Send((void*)msg.c_str(), msg.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			}
//			break;
		}
		if (strncmp(line.c_str(), "VmSwap:", 7) == 0) {
			if (mpiProcessRank == 0)  {
				cout << "Process " << mpiProcessRank << " swap mem usage: " << line.substr(7) << endl;

				for (int i = 1; i < mpiThreadCount; ++i) {
					MPI_Status status;
					MPI_Probe(i, 1, MPI_COMM_WORLD, &status);
					int l;
					MPI_Get_count(&status, MPI_CHAR, &l);
					char *buf = new char[l];
					MPI_Recv(buf, l, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
					cout << "Process " << i << " swap mem usage: " << buf << endl;
					delete [] buf;
				}
			} else {
				string msg = line.substr(7);
				MPI_Send((void*)msg.c_str(), msg.size(), MPI_CHAR, 0, 1, MPI_COMM_WORLD);
			}
//			break;
		}
	}
	statusFile.close();
//#endif
	MPI_Finalize();
#endif

	return 0;
}
