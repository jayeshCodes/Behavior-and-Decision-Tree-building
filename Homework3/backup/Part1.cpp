#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>

int main() {
    // Define the graph type
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;

    // Create a random graph with 100,000 vertices and approximately 500,000 edges
    Graph largeGraph;
    boost::generate_random_graph(largeGraph, 100000, 500000, 100000, true, false);

    // Print the number of vertices and edges
    std::cout << "Number of vertices: " << boost::num_vertices(largeGraph) << std::endl;
    std::cout << "Number of edges: " << boost::num_edges(largeGraph) << std::endl;

    return 0;
}
