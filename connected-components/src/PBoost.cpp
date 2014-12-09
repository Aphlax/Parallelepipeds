#include "PBoost.h"

#ifdef __linux__
// Copyright (C) 2004-2008 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Douglas Gregor
//           Andrew Lumsdaine

// Example usage of breadth_first_search algorithm

// Enable PBGL interfaces to BGL algorithms
#include <boost/graph/use_mpi.hpp>

// Communicate via MPI
#include <boost/graph/distributed/mpi_process_group.hpp>

// Breadth-first search algorithm
#include <boost/graph/breadth_first_search.hpp>

// Distributed adjacency list
#include <boost/graph/distributed/adjacency_list.hpp>

// METIS Input
#include <boost/graph/metis.hpp>

// Graphviz Output
#include <boost/graph/distributed/graphviz.hpp>

// For choose_min_reducer
#include <boost/graph/distributed/distributed_graph_utility.hpp>
#include <boost/property_map/parallel/distributed_property_map.hpp>

// Standard Library includes
#include <fstream>
#include <string>

#include <boost/graph/connected_components.hpp>
#include <ctime>
#include <chrono>
#include <algorithm>

#ifdef BOOST_NO_EXCEPTIONS
void
boost::throw_exception(std::exception const& ex)
{
    std::cout << ex.what() << std::endl;
    abort();
}
#endif

using namespace boost;
using boost::graph::distributed::mpi_process_group;

/* An undirected graph with distance values stored on the vertices. */
struct component_t {
        typedef vertex_property_tag kind;
};
typedef property<component_t, double> VertexComponent;
typedef adjacency_list<vecS, distributedS<mpi_process_group, vecS>, undirectedS, VertexComponent> Graph;

typedef graph_traits<Graph>::vertex_descriptor Vertex;

using namespace std;


int PBoost::run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {

	boost::mpi::environment env();


	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;
	start = std::chrono::system_clock::now();

	Graph g(numberOfVertices);
	if (process_id(g.process_group()) == 0) {
		int mpiProcessRank;
		MPI_Comm_rank (MPI_COMM_WORLD, &mpiProcessRank);
		// Only process 0 loads the graph, which is distributed automatically
		for (unsigned int i = 0; i < edges.size(); ++i) {
			add_edge(vertex(edges[i].first, g), vertex(edges[i].second, g), g);
		}
	}

	synchronize(g.process_group());
	if (process_id(g.process_group()) == 0) {
		cout << "Graph created\n";
		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoint 1: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();
	}
	property_map<Graph, component_t>::type component_map = get(component_t(), g);

	connected_components(g, component_map);

	if (process_id(g.process_group()) == 0) {
		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoint 2: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();

		cout << "Reading result\n";
		for (int i = 0; i < numberOfVertices; ++i) {
			outVertexToComponent[i] = get(component_map, vertex(i,g));
		}
	}
	synchronize(g.process_group());

	if (process_id(g.process_group()) == 0) {
		elapsed_seconds = std::chrono::system_clock::now()-start;
		cout << "Checkpoint 3: " << elapsed_seconds.count() << "s\n";
		start = std::chrono::system_clock::now();
	}
	return 0;

}
#else
int PBoost::run(const int numberOfVertices, const std::vector<std::pair<int,int> > &edges, std::vector<int> &outVertexToComponent) {
	return 0;
}
#endif
