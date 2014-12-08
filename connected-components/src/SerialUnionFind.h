
#ifndef __SERIAL_UNION_FIND_H_INCLUDED__
#define __SERIAL_UNION_FIND_H_INCLUDED__

# include <iostream>
# include <vector>
# include <utility>
# include <algorithm>
# include <queue>
# include "StopWatch.cpp"

class SerialUnionFind {

public:
	/**
	 * returns total number of components and fills out_vertex_to_component filled with each vertex's component.
	 * It expects that outVertexToComponent is a vector of size numberOfVertices, filled with -1
	 */
	int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent, StopWatch &stopWatch);
};

#endif
