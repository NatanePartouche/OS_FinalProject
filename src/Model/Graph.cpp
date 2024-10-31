#include "Graph.hpp"
#include <algorithm>
#include <iostream>

// Constructor to initialize a graph with a specified number of vertices.
// It resizes the adjacency list to the given number of vertices.
Graph::Graph(int vertices) {
    adjList.resize(vertices);
}

// Adds an undirected edge between vertices `u` and `v` with a specified weight.
// The edge is added to both `u` and `v`'s adjacency lists.
void Graph::addEdge(int u, int v, int weight) {
    if (isValidVertex(u) && isValidVertex(v)) {
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
    }
}

// Removes an undirected edge between vertices `u` and `v`.
// It searches for the edge in both `u`'s and `v`'s adjacency lists and removes it if found.
void Graph::removeEdge(int u, int v) {
    if (isValidVertex(u) && isValidVertex(v)) {
        auto& neighborsU = adjList[u];
        for (auto it = neighborsU.begin(); it != neighborsU.end(); ++it) {
            if (it->first == v) {
                neighborsU.erase(it);
                break;
            }
        }

        auto& neighborsV = adjList[v];
        for (auto it = neighborsV.begin(); it != neighborsV.end(); ++it) {
            if (it->first == u) {
                neighborsV.erase(it);
                break;
            }
        }
    }
}

// Changes the weight of an existing undirected edge between vertices `u` and `v` to `newWeight`.
// It updates the weight in both `u`'s and `v`'s adjacency lists.
void Graph::changeEdgeWeight(int u, int v, int newWeight) {
    if (isValidVertex(u) && isValidVertex(v)) {
        for (auto& neighbor : adjList[u]) {
            if (neighbor.first == v) {
                neighbor.second = newWeight;
            }
        }
        for (auto& neighbor : adjList[v]) {
            if (neighbor.first == u) {
                neighbor.second = newWeight;
            }
        }
    }
}

// Returns the total number of vertices in the graph.
int Graph::getNumVertices() {
    return static_cast<int>(adjList.size());
}

// Returns the total weight of all edges in the graph.
// Since each undirected edge is counted twice, the sum is divided by 2 to avoid double counting.
int Graph::getTotalWeight() {
    int totalWeight = 0;
    for (int u = 0; u < getNumVertices(); ++u) {
        for (const auto& neighbor : adjList[u]) {
            totalWeight += neighbor.second;  // Add the edge weight.
        }
    }
    return totalWeight / 2;
}

// Returns a constant reference to the adjacency list for accessing the graph structure externally.
const std::vector<std::list<std::pair<int, int>>>& Graph::getAdjList() {
    return adjList;
}

// Checks if a given vertex `v` is valid by ensuring it is within the range of defined vertices.
bool Graph::isValidVertex(int v) const {
    return v >= 0 && v < static_cast<int>(adjList.size());
}

// Provides a textual representation of the graph, showing all vertices and edges with weights.
std::string Graph::displayGraph() {
    std::string graphRepresentation;
    graphRepresentation += "============ Graph Representation ============\n";
    graphRepresentation += "Vertices in the graph: ";
    for (int i = 0; i < getNumVertices(); ++i) {
        graphRepresentation += std::to_string(i) + " ";
    }
    graphRepresentation += "\nConnections between vertices (undirected edges):\n";
    for (int i = 0; i < getNumVertices(); ++i) {
        for (const auto& neighbor : adjList[i]) {
            if (i < neighbor.first) {
                graphRepresentation += "Vertex " + std::to_string(i) + " <----(" + std::to_string(neighbor.second) + ")----> Vertex " + std::to_string(neighbor.first) + "\n";
            }
        }
    }
    graphRepresentation += "=============================================\n";
    return graphRepresentation;
}

// Compares this graph with another graph to check if they have the same structure and weights.
// It ensures that both graphs have the same vertices, total weight, and identical edges.
bool Graph::compareGraphs(Graph& other) {
    if (this->getNumVertices() != other.getNumVertices()) return false;
    if (this->getTotalWeight() != other.getTotalWeight()) return false;

    for (int i = 0; i < getNumVertices(); ++i) {
        const auto& thisNeighbors = this->adjList[i];
        const auto& otherNeighbors = other.adjList[i];

        // Check if each vertex has the same number of neighbors.
        if (thisNeighbors.size() != otherNeighbors.size()) return false;

        // Convert neighbors to sorted vectors for direct comparison.
        std::vector<std::pair<int, int>> sortedThisNeighbors(thisNeighbors.begin(), thisNeighbors.end());
        std::vector<std::pair<int, int>> sortedOtherNeighbors(otherNeighbors.begin(), otherNeighbors.end());

        std::sort(sortedThisNeighbors.begin(), sortedThisNeighbors.end());
        std::sort(sortedOtherNeighbors.begin(), sortedOtherNeighbors.end());

        // Compare sorted lists of edges for equality.
        if (sortedThisNeighbors != sortedOtherNeighbors) return false;
    }
    return true;
}