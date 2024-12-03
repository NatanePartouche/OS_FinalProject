#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <list>
#include <memory>
#include <utility>
#include <string>

/*
 * The Graph class represents an undirected weighted graph using an adjacency list structure.
 *
 * Adjacency List (adjList) Structure and Explanation:
 * The adjacency list is a commonly used data structure to represent graphs efficiently, especially when the graph is sparse
 * (i.e., has relatively few edges compared to the number of vertices). An adjacency list provides a compact way to store
 * vertices and edges by keeping track of only the neighboring vertices for each vertex.
 * In this implementation:
 * 1. `std::vector<std::list<std::pair<int, int>>> adjList;`
 *    • The `adjList` is a vector where each element corresponds to a vertex in the graph.
 *    • Each element of the vector, `adjList[i]`, represents the list of edges connected to vertex `i`.
 *    • Each edge in `adjList[i]` is represented as a `std::pair<int, int>`, where:
 *        - The first element in the pair (an `int`) is a neighboring vertex connected to vertex `i`.
 *        - The second element in the pair (an `int`) represents the weight or cost of the edge between vertex `i`
 *          and that neighboring vertex.
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
public:
    // Vector where each index represents a vertex, and each element is a list of pairs representing edges.
    std::vector<std::list<std::pair<int, int>>> adjList;
    std::string _algorithmChoice = "prim";
    std::unique_ptr<Graph> mst;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//                          Functions primarily used for random Graph                                //
///////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructor to initialize a graph with a given number of vertices.
    Graph(int vertices);

    // Copy constructor
    Graph(const Graph& other);
    // Copy assignment operator
    Graph& operator=(const Graph& other);
    // Move assignment operator
    Graph& operator=(Graph&& other) noexcept;
    // Adds an edge between vertices `u` and `v` with the specified weight.
    void add_edge(int u, int v, int weight);
    // Removes an edge between vertices `u` and `v`.
    void remove_edge(int u, int v);
    // Returns the total number of vertices in the graph.
    int getNumVertices();
    // Returns a constant reference to the adjacency list, allowing access to the graph's structure.
    const std::vector<std::list<std::pair<int, int>>>& getAdjList();
    // Checks if a given vertex `v` is valid (within the range of defined vertices).
    bool isValidVertex(int v) const;
    // Compares this graph with another graph to see if they have the same structure and weights.
    bool compareGraphs(Graph& other);
    // Changes the weight of an existing undirected edge between vertices `u` and `v` to `newWeight`.
    void changeEdgeWeight(int u, int v, int newWeight);
///////////////////////////////////////////////////////////////////////////////////////////////////////
//            Functions primarily used for MST (Minimum Spanning Tree) operations                    //
///////////////////////////////////////////////////////////////////////////////////////////////////////
    // Returns the total weight of all edges in the MST.
    double getTotalWeight();
    // Returns the total weight of all edges in the MST.
    double getTotalWeight_MST();
    // Displays the graph structure, showing each vertex and its connected edges.
    std::string displayGraph();
    // Displays the MST structure, showing each vertex and its connected edges.
    std::string displayMST();
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
    // Performs a comprehensive analysis of the graph and its MST and stores the results
    std::string Analysis();
    /* The Solve method is designed to execute the primary algorithm associated with the graph.
     * Depending on the context, this method could:
     *  - Construct the Minimum Spanning Tree (MST) of the graph using the algorithm specified
     *    by the `_algorithmChoice` member variable (e.g., "prim" for Prim's algorithm).
     *  - Analyze or manipulate the graph in a way defined by the user.
     *  - Act as an entry point for solving a particular problem using the graph's structure.
     *
     * This function encapsulates the core logic and ensures that the graph's state is updated
     * or results are computed based on the input and selected algorithm.
     *
     * The results or changes performed by this function can be accessed through other member functions
     * such as `displayGraph`, `displayMST`, or `Analysis`*/
    void Solve();

};
#endif // GRAPH_HPP