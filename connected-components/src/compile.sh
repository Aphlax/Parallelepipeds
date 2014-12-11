non-mpi:
icc -mmic -fopenmp main.cpp Boost.cpp RandomGraph.cpp Bfs.cpp OpenMPCC.cpp pBoost.cpp SerialUnionFind.cpp RandomizedContract.cpp PRandomizedContract.cpp SpanningTreeCC.cpp pBfsAtomic.cpp -std=c++11 -o pcc.mic -I"../boost/boost_1_57_0" 


mpi (our own which does not work):
/home/dphpc19/mpi/bin/mpic++ -mmic -fopenmp main.cpp Boost.cpp RandomGraph.cpp Bfs.cpp OpenMPCC.cpp pBoost.cpp SerialUnionFind.cpp RandomizedContract.cpp PRandomizedContract.cpp SpanningTreeCC.cpp pBfsAtomic.cpp -std=c++11 -o pcc.mic -I"../boost/boost_1_57_0" 

mpi (sent by timo, not tested!):
/opt/intel/impi/5.0.1.035/bin64/mpiicc -mmic -fopenmp main.cpp Boost.cpp RandomGraph.cpp Bfs.cpp OpenMPCC.cpp pBoost.cpp SerialUnionFind.cpp RandomizedContract.cpp PRandomizedContract.cpp SpanningTreeCC.cpp pBfsAtomic.cpp -std=c++11 -o pcc.mic -I"../boost/boost_1_57_0" 


sbatch ./run.sh