#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"

#include "RandomGraph.h"
#include <queue>

using namespace std;

void print(vector<int> &v){
	if (v.size() == 0) {
		cout << "empty!\n";
	}
	for (unsigned int i = 0; i < v.size(); ++i) {
		cout << v[i] << (i == v.size() - 1 ? "\n" : ", ");
	}
}

void print(std::vector<std::set<int> > &g){
	if (g.size() == 0) {
		cout << "empty!\n";
	}
	for (unsigned int i = 0; i < g.size(); ++i) {
		cout << i << ": ";
		for (set<int>::iterator it = g[i].begin(); it != g[i].end(); ++it) {
			cout << *it << " ";
		}
		cout << "\n";
	}
}

void parseGraphString(std::string s, std::vector<std::set<int> >& graph, std::vector<int>& vertexEdgesCount) {
	size_t pos = 0;
	std::string delimiter = "\n";
	bool firstLine = true;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		// a line
		std::string line = s.substr(0, pos);
		size_t pos2 = line.find(" ");
		std::string first = line.substr(0, pos2);
		std::string second = line.substr(pos2 + 1);
		int firstInt = atoi(first.c_str());
		int secondInt = atoi(second.c_str());
		if (firstLine) {
			graph.resize(firstInt, set<int>());
			vertexEdgesCount.resize(firstInt, 0);
			firstLine = false;
		} else {
			graph[firstInt].insert(secondInt);
			graph[secondInt].insert(firstInt);
			++vertexEdgesCount[firstInt];
		}
		s.erase(0, pos + delimiter.length());
	}
}

void checkGraph(std::string s, unsigned int expectedSize, vector<int> &expectedSizeOfEachComponent, int expectedAvgConnectivity, int expectedComponents) {
	cout << "Checking graph\n";

	std::vector<std::set<int> > graph;
	std::vector<int > vertexEdgesCount; // each edge is only counted once
	parseGraphString(s, graph, vertexEdgesCount);
//	print(graph);

	ASSERT_EQUAL(expectedSize, graph.size());

	vector<int> component(graph.size(), -1);
	vector<int> componentSizes;
	for (unsigned int i = 0; i < graph.size(); ++i) {
		if (component[i] >= 0) continue;

		int curComponentSize = 0;
		queue<int> q;
		q.push(i);
		while(!q.empty()) {
			int cur = q.front();
			q.pop();
			if (component[cur] >= 0) continue;
			component[cur] = componentSizes.size();
			++curComponentSize;

			for (set<int>::iterator it = graph[cur].begin(); it != graph[cur].end(); ++it) {
				int next = *it;
				if (component[next] >= 0) continue;
				q.push(next);
			}
		}
		componentSizes.push_back(curComponentSize);
	}

	vector<int> sortedExpectedSizeOfEachComponent = expectedSizeOfEachComponent;
	sort(sortedExpectedSizeOfEachComponent.begin(), sortedExpectedSizeOfEachComponent.end());
	vector<int> sortedComponentSizes = componentSizes;
	sort(sortedComponentSizes.begin(), sortedComponentSizes.end());
	cout << "Expected: ";
	print(sortedExpectedSizeOfEachComponent);
	cout << "Actual: ";
	print(sortedComponentSizes);
	ASSERT_EQUAL(sortedExpectedSizeOfEachComponent, sortedComponentSizes);

	vector<int> expectedEdges;
	for (unsigned int i = 0; i < componentSizes.size(); ++i) {
		int cn = componentSizes[i];
		expectedEdges.push_back(min((int) (expectedAvgConnectivity * cn),	(cn * (cn - 1)) / 2));
	}

	vector<int> edgeCount(componentSizes.size(), 0);
	for (unsigned int i = 0; i < graph.size(); ++i) {
		edgeCount[component[i]] += vertexEdgesCount[i];
	}

	cout << "Expected: ";
	print(expectedEdges);
	cout << "Actual: ";
	print(edgeCount);
	ASSERT_EQUAL(expectedEdges, edgeCount);
}

void graphTest1Vertex1Component() {
	int expectedConnectivity = 3;
	vector<int> sizeOfEachComponent;
	sizeOfEachComponent.push_back(1);

	RandomGraph r(sizeOfEachComponent, expectedConnectivity);
	checkGraph(r.toString(), 1, sizeOfEachComponent, expectedConnectivity, 1);

}

void graphTest1Vertex2Component() {
	int expectedConnectivity = 3;
	vector<int> sizeOfEachComponent;
	sizeOfEachComponent.push_back(1);
	sizeOfEachComponent.push_back(1);

	RandomGraph r(sizeOfEachComponent, expectedConnectivity);
	checkGraph(r.toString(), 2, sizeOfEachComponent, expectedConnectivity, 2);
}

void graphTest2Vertex1Component() {
	int expectedConnectivity = 3;
	vector<int> sizeOfEachComponent;
	sizeOfEachComponent.push_back(2);

	RandomGraph r(sizeOfEachComponent, expectedConnectivity);
	checkGraph(r.toString(), 2, sizeOfEachComponent, expectedConnectivity, 1);
}

void graphTest2Vertex2Component() {
	int expectedConnectivity = 3;
	vector<int> sizeOfEachComponent;
	sizeOfEachComponent.push_back(2);
	sizeOfEachComponent.push_back(2);

	RandomGraph r(sizeOfEachComponent, expectedConnectivity);
	checkGraph(r.toString(), 4, sizeOfEachComponent, expectedConnectivity, 2);
}

void graphTest1000Vertex10Component() {
	vector<int> sizeOfEachComponent;
	int expectedSize = 0;
	int expectedConnectivity = 3;
	int expectedComponentCount = 10;
	for (int i = 0; i < expectedComponentCount; ++i) {
		sizeOfEachComponent.push_back(1000);
		expectedSize += sizeOfEachComponent.back();
	}

	RandomGraph r(sizeOfEachComponent, expectedConnectivity);
	checkGraph(r.toString(), expectedSize, sizeOfEachComponent, expectedConnectivity, expectedComponentCount);
}

void graphTestVariableVertex10Component() {
	vector<int> sizeOfEachComponent;
	int expectedSize = 0;
	int expectedConnectivity = 10;
	int expectedComponentCount = 10;
	for (int i = 0; i < expectedComponentCount; ++i) {
		sizeOfEachComponent.push_back(100*(i+1));
		expectedSize += sizeOfEachComponent.back();
	}

	RandomGraph r(sizeOfEachComponent, expectedConnectivity);
	checkGraph(r.toString(), expectedSize, sizeOfEachComponent, expectedConnectivity, expectedComponentCount);
}

void runAllTests(int argc, char const *argv[]){
	cute::suite s;
	//TODO add your test here
	s.push_back(CUTE(graphTest1Vertex1Component));
	s.push_back(CUTE(graphTest1Vertex2Component));
	s.push_back(CUTE(graphTest2Vertex1Component));
	s.push_back(CUTE(graphTest2Vertex2Component));
	s.push_back(CUTE(graphTest1000Vertex10Component));
	s.push_back(CUTE(graphTestVariableVertex10Component));
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<> >  lis(xmlfile.out);
	cute::makeRunner(lis,argc,argv)(s, "AllTests");
}

int main(int argc, char const *argv[]){
    runAllTests(argc,argv);
    return 0;
}



