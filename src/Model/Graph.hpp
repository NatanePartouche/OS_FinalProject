#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <list>
#include <utility>
#include <string>

/*
 * The Graph class represents an undirected weighted graph using an adjacency list structure.
 *
 * Adjacency List (adjList) Structure and Explanation:
 *
 * The adjacency list is a commonly used data structure to represent graphs efficiently, especially when the graph is sparse
 * (i.e., has relatively few edges compared to the number of vertices). An adjacency list provides a compact way to store
 * vertices and edges by keeping track of only the neighboring vertices for each vertex.
 *
 * In this implementation:
 *
 * 1. `std::vector<std::list<std::pair<int, int>>> adjList;`
 *    • The `adjList` is a vector where each element corresponds to a vertex in the graph.
 *    • Each element of the vector, `adjList[i]`, represents the list of edges connected to vertex `i`.
 *    • Each edge in `adjList[i]` is represented as a `std::pair<int, int>`, where:
 *        - The first element in the pair (an `int`) is a neighboring vertex connected to vertex `i`.
 *        - The second element in the pair (an `int`) represents the weight or cost of the edge between vertex `i`
 *          and that neighboring vertex.
 *
 * Example of the adjacency list structure:
 * For a graph with vertices 0, 1, and 2, and edges between them:
 *      - Edge (0, 1) with weight 3
 *      - Edge (1, 2) with weight 5
 *      - Edge (0, 2) with weight 7
 * The adjacency list `adjList` would look like:
 *      adjList[0] -> {(1, 3), (2, 7)}
 *      adjList[1] -> {(0, 3), (2, 5)}
 *      adjList[2] -> {(0, 7), (1, 5)}
 * In this example:
 *  - `adjList[0]` contains a list of pairs representing edges from vertex 0 to vertices 1 and 2 with weights 3 and 7, respectively.
 *  - This structure is efficient for quickly accessing the neighbors of any vertex and is widely used in graph algorithms.
 */

class Graph {
protected:
    // Vector where each index represents a vertex, and each element is a list of pairs representing edges.
    std::vector<std::list<std::pair<int, int>>> adjList;

public:
    Graph(int vertices); // Constructor to initialize a graph with a given number of vertices.
    ~Graph();
    void add_edge_on_Graph(int u, int v, int weight);
    void remove_edge_on_Graph(int u, int v);
    // Changes the weight of the edge between vertices `u` and `v` to `newWeight`.
    void changeEdgeWeight(int u, int v, int newWeight);
    // Displays the graph structure, showing each vertex and its connected edges.
    std::string displayGraph();
    // Returns the total number of vertices in the graph.
    int getNumVertices();
    // Returns a constant reference to the adjacency list, allowing access to the graph's structure.
    const std::vector<std::list<std::pair<int, int>>>& getAdjList();
    // Checks if a given vertex `v` is valid (within the range of defined vertices).
    bool isValidVertex(int v) const;
    // Compares this graph with another graph to see if they have the same structure and weights.
    bool compareGraphs(Graph& other);
    Graph& operator=(const Graph& other);


    // Returns the total weight of all edges in the graph.
    int getTotalWeight();
    // Finds the longest path in the MST (returns a string representing the path in the format "0->9->...").
    std::string getTreeDepthPath_MST();
    // Retrieves the heaviest edge in the MST (returns a string in the format "u v w",
    // where u and v are the vertices connected by the edge, and w is the edge weight).
    std::string getMaxWeightEdge_MST();
    // Finds the heaviest path in the MST and returns it as a string.
    std::string getMaxWeightPath_MST();
    // Retrieves the lightest edge in the MST (returns a string in the format "u v w").
    std::string getMinWeightEdge_MST();
    // Calculates the average distance between all pairs of vertices (Xi, Xj) in the MST.
    double getAverageDistance_MST();
    // Function to find the path between two vertices in an MST without calling a helper function
    std::string findPath_MST(int u, int v);

};

#endif // GRAPH_HPP