
#ifndef __SERIAL_CONNECTED_COMPONENTS_H_INCLUDED__
#define __SERIAL_CONNECTED_COMPONENTS_H_INCLUDED__

#include <vector>

class SerialConnectedComponents {

public:
	/**
	 * returns total number of components and fills out_vertex_to_component filled with each vertex's component.
	 * It expects that outVertexToComponent is a vector of size numberOfVertices, filled with -1
	 */
	int run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent);
};

#endif
