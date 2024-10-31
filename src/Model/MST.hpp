#ifndef MST_HPP
#define MST_HPP

#include "Graph.hpp"
#include <iostream>
#include <string>

/*
 * The MST (Minimum Spanning Tree) class inherits from the Graph class and provides
 * functionalities to generate and analyze the minimum spanning tree of a given graph.
 */
class MST : public Graph {

public:
    MST();
    MST(Graph& graph, std::string Type_of_Algo);

    // 1. Returns the total weight of the minimum spanning tree (MST) as a double.
    double getTotalWeight();

    // 2. Finds the longest path in the MST (returns a string representing the path in the format "0->9->...").
    std::string getTreeDepthPath();

    // 3. Retrieves the heaviest edge in the MST (returns a string in the format "u v w",
    //    where u and v are the vertices connected by the edge, and w is the edge weight).
    std::string getMaxWeightEdge();

    // 4. Finds the heaviest path in the MST and returns it as a string.
    std::string getMaxWeightPath();

    // 5. Calculates the average distance between all pairs of vertices (Xi, Xj) in the MST.
    double getAverageDistance();

    // 6. Retrieves the lightest edge in the MST (returns a string in the format "u v w").
    std::string getMinWeightEdge();

    // Adds an edge and checks if it can improve the MST.
    // Returns true if the addition improves or maintains the MST, false otherwise.
    bool add(int u, int v, int weight);

    // Removes an edge and ensures that the graph remains a valid MST.
    // Returns true if the removal maintains the MST, false otherwise.
    bool remove(int u, int v);
};

#endif  // MST_HPP