
#ifndef __RANDOM_GRAPH_H_INCLUDED__
#define __RANDOM_GRAPH_H_INCLUDED__

#include <vector>
#include <string>
#include <set>

class RandomGraph {

private:
	int edgeCount;
	std::vector<std::set<int> > vertexToVertices;


	bool connect(std::vector<std::set<int> > &g, const int a, const int b);
	void randomize();
	void print(std::vector<int> &v);
public:
	std::vector<std::set<int> > getGraphDatastructure() const {
		return std::vector<std::set<int> >();
	}

	RandomGraph(const std::vector<int> &sizeOfEachComponent, const double averageConnectivity);
	std::string toString();
	int getEdgeCount();
	std::vector<std::set<int> >* getGraphDatastructure();
	std::vector<std::pair<int, int> > getEdgeList();
};

#endif
