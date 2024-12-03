#ifndef MSTFACTORY_HPP
#define MSTFACTORY_HPP

class Graph;
#include <vector>


class MSTFactory {
public:
    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~MSTFactory() = default;
    /*
     * Pure virtual function to solve the MST problem. This method must be implemented by all derived classes.
     */
    virtual Graph solveMST(Graph& graph) = 0;
};

/*
 * PrimSolver:
 * A class that implements Prim's algorithm for computing the Minimum Spanning Tree (MST).
 * Prim's algorithm builds the MST by starting from an arbitrary vertex and repeatedly adding the smallest edge
 * that connects a vertex in the MST to a vertex outside the MST.
 */
class PrimSolver : public MSTFactory {
public:
    Graph solveMST(Graph& graph) override;
};

/*
 * KruskalSolver:
 * A class that implements Kruskal's algorithm for computing the Minimum Spanning Tree (MST).
 * Kruskal's algorithm works by sorting all the edges in the graph by weight and adding them to the MST one by one,
 * ensuring that no cycles are formed. This process continues until the MST is complete.
 */
class KruskalSolver : public MSTFactory {
public:
    Graph solveMST(Graph& graph) override;
};

/*
 * BoruvkaSolver:
 * A class that implements Borůvka's algorithm for computing the Minimum Spanning Tree (MST).
 * Borůvka's algorithm is highly parallelizable and works by repeatedly merging connected components using the
 * smallest edge from each component, until only one component (the MST) remains.
 */
class BoruvkaSolver : public MSTFactory {
public:
    Graph solveMST(Graph& graph) override;
};

/*
 * TarjanSolver:
 * A class that implements Tarjan's algorithm for computing the Minimum Spanning Tree (MST).
 * Tarjan's algorithm is often used in dynamic or offline MST problems and is less commonly applied in basic MST problems.
 * This solver provides a solution for more advanced graph scenarios.
 */
class TarjanSolver : public MSTFactory {
public:
    Graph solveMST(Graph& graph) override;
};

/*
 * IntegerMSTSolver:
 * A class that implements a specialized MST algorithm for graphs with integer edge weights.
 * This solver can take advantage of integer-specific optimizations to compute the MST efficiently.
 */
class IntegerMSTSolver : public MSTFactory {
public:
    Graph solveMST(Graph& graph) override;
};


// Union-Find with path compression and union by rank
class UnionFind {
public:
    UnionFind(int n);
    ~UnionFind();
    int find(int u);
    bool unionSets(int u, int v);

private:
    std::vector<int> parent;
    std::vector<int> rank;
};

#endif  // MSTFACTORY_HPP