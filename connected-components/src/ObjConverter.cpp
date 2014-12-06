#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <queue>
#include <vector>
#include <string.h>
#include <ctype.h>
#include <cstdlib>
#include <stdexcept>   // for exception, runtime_error, out_of_range
#include <algorithm>

using namespace std;

// ferrari from http://www.turbosquid.com/3d-models/599gtb-midres-3d-model/483862
// space station from http://www.turbosquid.com/3d-models/scifi-structure-max-free/742663
class ObjConverter {
public:
	void convert(const string& inFile, const string& outFile) {
		ifstream objFile(inFile);
		if (!objFile.is_open())
			throw std::runtime_error("Unable to open obj file");

		cout << "Reading obj file...\n";
		set< int > vertices;
		set< pair<int, int> > edges;
		string line;
		int count = 0;
		int maxVertexId = 0;
		while (getline(objFile, line)) {
			if (strncmp(line.c_str(), "f ", 2) == 0) {
				++count;

				if (count > 2216090) cout << line << endl;

				int prevV = -1;
				int firstV = -1;
				int faceVertexCount = 0;
				std::vector<std::string> tokens = split(line, ' ');
				for (unsigned int i = 1; i < tokens.size(); ++i) {
					std::vector<std::string> vIdxs = split(tokens[i], '/');
					if(vIdxs[0].find_first_not_of(' ') == std::string::npos) continue; // check if string is blank

					int v = atoi(vIdxs[0].c_str());
					if (count > 2216090) cout << v << ": "<< vIdxs[0] << endl;
					if (v == 0) cout << v << ": "<< vIdxs[0] << endl;

					vertices.insert(v);
					maxVertexId = max(v,maxVertexId);
					if (prevV == -1) {
						firstV = v;
					} else {
						edges.insert(pair<int, int>(min(prevV, v), max(prevV, v)));
						if (count > 2216090) cout << "inserting: " << prevV << ", " << v << endl;
					}
					if (i == tokens.size() - 1) {
						edges.insert(pair<int, int>(min(v, firstV), max(v, firstV)));
						if (count > 2216090) cout << "inserting: " << v << ", " << firstV << endl;
					}
					prevV = v;
					++faceVertexCount;
					if (faceVertexCount == 4) count++;
				}
			}
		}
		cout << count << " faces, "  << vertices.size() << " vertices and "  << edges.size() << " edges" << endl << endl;
		objFile.close();

		// check if no vertex is missing
		vector<int> newVertexId(maxVertexId + 1, -1);
		int idx = 0;
		for (set< int >::iterator it = vertices.begin(); it != vertices.end(); ++it) {
			newVertexId[*it] = idx++;
		}



		// save graph
		cout << "Saving graph...\n";
		ofstream graphFile(outFile.c_str());

		if (!graphFile.is_open()) throw std::runtime_error ("Unable to open file: " + outFile);

		graphFile << vertices.size() << " " << edges.size() << endl;
		for (set< pair<int, int> >::iterator it = edges.begin(); it != edges.end(); ++it) {
			graphFile << newVertexId[it->first] << " " << newVertexId[it->second] << endl;
		}
		graphFile.close();

		cout << "Graph saved at: " << outFile << "\n";

	}
private:
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	    std::stringstream ss(s);
	    std::string item;
	    while (std::getline(ss, item, delim)) {
	        elems.push_back(item);
	    }
	    return elems;
	}


	std::vector<std::string> split(const std::string &s, char delim) {
	    std::vector<std::string> elems;
	    split(s, delim, elems);
	    return elems;
	}
};
