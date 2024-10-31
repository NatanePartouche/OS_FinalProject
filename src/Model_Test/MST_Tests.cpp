#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../src/Model_Test/doctest.h"
#include "../../src/Model/Graph.hpp"
#include "../../src/Model/MST.hpp"

// Test case for the constructor and initial properties
TEST_CASE("Graph: Constructor and Initial State") {
    Graph g(5);
    CHECK(g.getNumVertices() == 5);
    CHECK(g.getTotalWeight() == 0);
    CHECK(g.getAdjList().size() == 5);
}

// Test case for adding edges and verifying the graph structure
TEST_CASE("Graph: Adding Edges") {
    Graph g(4);

    g.addEdge(0, 1, 10);
    g.addEdge(0, 2, 5);
    g.addEdge(1, 2, 7);

    CHECK(g.getTotalWeight() == 22);
    CHECK(g.getAdjList()[0].size() == 2);
    CHECK(g.getAdjList()[1].size() == 2);
    CHECK(g.getAdjList()[2].size() == 2);

    // Verify adjacency lists
    CHECK(g.getAdjList()[0].front().first == 1);
    CHECK(g.getAdjList()[0].front().second == 10);
    CHECK(g.getAdjList()[0].back().first == 2);
    CHECK(g.getAdjList()[0].back().second == 5);
}

// Test case for removing edges
TEST_CASE("Graph: Removing Edges") {
    Graph g(4);

    g.addEdge(0, 1, 10);
    g.addEdge(0, 2, 5);
    g.addEdge(1, 2, 7);

    g.removeEdge(0, 1);

    // Check the total weight after removal
    CHECK(g.getTotalWeight() == 12);  // 5 + 7

    // Check that the edge 0-1 is removed
    CHECK(g.getAdjList()[0].size() == 1);
    CHECK(g.getAdjList()[1].size() == 1);
}

// Test case for changing the edge weight
TEST_CASE("Graph: Changing Edge Weights") {
    Graph g(4);

    g.addEdge(0, 1, 10);
    g.addEdge(0, 2, 5);
    g.addEdge(1, 2, 7);

    // Modify the weight of the edge between 0 and 2
    g.changeEdgeWeight(0, 2, 15);

    CHECK(g.getTotalWeight() == 32);  // 10 + 15 + 7

    // Check the new weight in the adjacency list
    CHECK(g.getAdjList()[0].back().second == 15);
}

// Test case for checking vertex validity
TEST_CASE("Graph: Vertex Validity") {
    Graph g(5);
    CHECK(g.isValidVertex(0) == true);
    CHECK(g.isValidVertex(4) == true);
    CHECK(g.isValidVertex(5) == false);
    CHECK(g.isValidVertex(-1) == false);
}

// Test case for recalculating total weight after modifications
TEST_CASE("Graph: Recalculate Total Weight") {
    Graph g(3);

    g.addEdge(0, 1, 4);
    g.addEdge(1, 2, 6);

    CHECK(g.getTotalWeight() == 10);

    // Change the weight of the edge between 0 and 1
    g.changeEdgeWeight(0, 1, 8);

    CHECK(g.getTotalWeight() == 14);

    // Remove an edge and check the total weight
    g.removeEdge(1, 2);
    CHECK(g.getTotalWeight() == 8);
}

// Test case for an empty graph
TEST_CASE("Graph: Empty Graph") {
    Graph g(0);  // Graph with 0 vertices
    CHECK(g.getNumVertices() == 0);
    CHECK(g.getTotalWeight() == 0);

    // Trying to add an edge to a graph with no vertices should do nothing
    g.addEdge(0, 1, 10);
    CHECK(g.getTotalWeight() == 0);
    CHECK(g.getAdjList().size() == 0);  // No vertices to add edges to
}

// Test case for invalid operations
TEST_CASE("Graph: Invalid Operations") {
    Graph g(3);

    // Attempt to add/remove/change edges with invalid vertex indices
    g.addEdge(0, 3, 10);  // Vertex 3 does not exist
    g.addEdge(-1, 2, 5);  // Negative index
    CHECK(g.getTotalWeight() == 0);  // No edges should have been added

    g.removeEdge(0, 3);  // Invalid removal
    g.changeEdgeWeight(0, 3, 20);  // Invalid weight change
    CHECK(g.getTotalWeight() == 0);  // Total weight should remain 0
}

// Test case for the default constructor
TEST_CASE("MST: Default Constructor") {
    MST mst;
    CHECK(mst.getNumVertices() == 0);
    CHECK(mst.getTotalWeight() == 0);
}

TEST_CASE("Test MST with an undirected graph of 3 vertices") {
    // Create a test graph
    Graph graph(3);
    graph.addEdge(0, 1, 2);  // 0 - 1
    graph.addEdge(1, 2, 3);  // 1 - 2
    graph.addEdge(0, 2, 10); // 0 - 2

    // Expected Minimum Spanning Tree
    Graph expectedMST(3);
    expectedMST.addEdge(0, 1, 2);  // 0 - 1
    expectedMST.addEdge(1, 2, 3);  // 1 - 2

    std::string temp = "PRIM";

    // Test with each MST algorithm
    MST mstPrim(graph, temp);
    CHECK(mstPrim.compareGraphs(expectedMST));

    temp = "KRUSKAL";

    MST mstKruskal(graph,temp);
    CHECK(mstKruskal.compareGraphs(expectedMST));

    temp = "BORUVKA";

    MST mstBoruvka(graph, temp);
    CHECK(mstBoruvka.compareGraphs(expectedMST));

    temp = "TARJAN";

    MST mstTarjan(graph, temp);
    CHECK(mstTarjan.compareGraphs(expectedMST));

    temp = "INTEGER_MST";

    MST mstInteger(graph,temp);
    CHECK(mstInteger.compareGraphs(expectedMST));
}

TEST_CASE("Test MST with an undirected graph of 5 vertices") {
    Graph graph(5);
    graph.addEdge(0, 1, 2);
    graph.addEdge(1, 2, 3);
    graph.addEdge(0, 3, 6);
    graph.addEdge(1, 4, 5);
    graph.addEdge(3, 1, 8);
    graph.addEdge(4, 2, 7);

    Graph expectedMST(5);
    expectedMST.addEdge(0, 1, 2);
    expectedMST.addEdge(1, 2, 3);
    expectedMST.addEdge(0, 3, 6);
    expectedMST.addEdge(1, 4, 5);

    std::string temp = "PRIM";

    // Test with each MST algorithm
    MST mstPrim(graph, temp);
    CHECK(mstPrim.compareGraphs(expectedMST));

    temp = "KRUSKAL";

    MST mstKruskal(graph,temp);
    CHECK(mstKruskal.compareGraphs(expectedMST));

    temp = "BORUVKA";

    MST mstBoruvka(graph, temp);
    CHECK(mstBoruvka.compareGraphs(expectedMST));

    temp = "TARJAN";

    MST mstTarjan(graph, temp);
    CHECK(mstTarjan.compareGraphs(expectedMST));

    temp = "INTEGER_MST";

    MST mstInteger(graph,temp);
    CHECK(mstInteger.compareGraphs(expectedMST));
}

TEST_CASE("Test MST with an undirected graph of 6 vertices") {
    Graph graph(6);
    graph.addEdge(0, 1, 6);
    graph.addEdge(1, 3, 2);
    graph.addEdge(3, 5, 8);
    graph.addEdge(5, 4, 8);
    graph.addEdge(4, 0, 9);
    graph.addEdge(2, 0, 3);
    graph.addEdge(2, 1, 4);
    graph.addEdge(2, 3, 2);
    graph.addEdge(2, 5, 9);
    graph.addEdge(2, 4, 9);

    Graph expectedMST(6);
    expectedMST.addEdge(3, 1, 2);
    expectedMST.addEdge(3, 2, 2);
    expectedMST.addEdge(3, 5, 8);
    expectedMST.addEdge(5, 4, 8);
    expectedMST.addEdge(2, 0, 3);

    std::string temp = "PRIM";

    // Test with each MST algorithm
    MST mstPrim(graph, temp);
    CHECK(mstPrim.compareGraphs(expectedMST));

    temp = "KRUSKAL";

    MST mstKruskal(graph,temp);
    CHECK(mstKruskal.compareGraphs(expectedMST));

    temp = "BORUVKA";

    MST mstBoruvka(graph, temp);
    CHECK(mstBoruvka.compareGraphs(expectedMST));

    temp = "TARJAN";

    MST mstTarjan(graph, temp);
    CHECK(mstTarjan.compareGraphs(expectedMST));

    temp = "INTEGER_MST";

    MST mstInteger(graph,temp);
    CHECK(mstInteger.compareGraphs(expectedMST));
}

TEST_CASE("Test MST with a disconnected graph of 6 vertices") {
    Graph disconnectedGraph(6);
    disconnectedGraph.addEdge(0, 1, 6);
    disconnectedGraph.addEdge(2, 3, 2);
    disconnectedGraph.addEdge(4, 5, 8);

    Graph expectedEmptyGraph(0);

    std::string temp = "PRIM";

    MST mstPrim(disconnectedGraph, temp);
    CHECK(mstPrim.compareGraphs(expectedEmptyGraph));

    temp = "KRUSKAL";

    MST mstKruskal(disconnectedGraph, temp);
    CHECK(mstKruskal.compareGraphs(expectedEmptyGraph));

    temp = "BORUVKA";

    MST mstBoruvka(disconnectedGraph, temp);
    CHECK(mstBoruvka.compareGraphs(expectedEmptyGraph));

    temp = "TARJAN";

    MST mstTarjan(disconnectedGraph, temp);
    CHECK(mstTarjan.compareGraphs(expectedEmptyGraph));

    temp = "INTEGER_MST";

    MST mstInteger(disconnectedGraph, temp);
    CHECK(mstInteger.compareGraphs(expectedEmptyGraph));
}

// Test case for getTotalWeight in an MST
TEST_CASE("MST: Total Weight Calculation") {
    // Create a simple graph
    Graph g(4);
    g.addEdge(0, 1, 10);
    g.addEdge(0, 2, 5);
    g.addEdge(1, 2, 7);
    g.addEdge(2, 3, 3);

    std::string temp = "PRIM";

    // Create an MST using Prim's algorithm
    MST mstPrim(g, temp);

    // Verify the total weight of the MST using Prim's algorithm
    CHECK(mstPrim.getTotalWeight() == 15);  // Edges selected: (0-2), (2-3), (1-2)

    temp = "KRUSKAL";

    // Create an MST using Kruskal's algorithm
    MST mstKruskal(g, temp);

    // Verify the total weight of the MST using Kruskal's algorithm
    CHECK(mstKruskal.getTotalWeight() == 15);  // Edges selected: (0-2), (2-3), (1-2)
}

TEST_CASE("BIG TESTS") {

// --- Test graph with 3 vertices ---
Graph graph3(3);
graph3.addEdge(0, 1, 2);
graph3.addEdge(1, 2, 3);
graph3.addEdge(0, 2, 10);

Graph expectedMST3(3);
expectedMST3.addEdge(0, 1, 2);
expectedMST3.addEdge(1, 2, 3);

std::cout << "\n//////////////////////////////////////////////////////////////////////////" << std::endl;
std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
std::cout << "--- Tests for graph3 ---" << std::endl;

MST mstPrim3(graph3, "PRIM");
MST mstKruskal3(graph3, "KRUSKAL");
MST mstBoruvka3(graph3, "BORUVKA");
MST mstTarjan3(graph3, "TARJAN");
MST mstInteger3(graph3, "INTEGER_MST");

if (mstPrim3.compareGraphs(expectedMST3)) std::cout << "PRIM: Success" << std::endl; else std::cout << "PRIM: Failure" << std::endl;
if (mstKruskal3.compareGraphs(expectedMST3)) std::cout << "KRUSKAL: Success" << std::endl; else std::cout << "KRUSKAL: Failure" << std::endl;
if (mstBoruvka3.compareGraphs(expectedMST3)) std::cout << "BORUVKA: Success" << std::endl; else std::cout << "BORUVKA: Failure" << std::endl;
if (mstTarjan3.compareGraphs(expectedMST3)) std::cout << "TARJAN: Success" << std::endl; else std::cout << "TARJAN: Failure" << std::endl;
if (mstInteger3.compareGraphs(expectedMST3)) std::cout << "INTEGER_MST: Success" << std::endl; else std::cout << "INTEGER_MST: Failure" << std::endl;

std::cout << mstPrim3.displayGraph() << std::endl;

// Property tests for graph3
std::cout << "Expected total weight: 5, obtained: " << mstPrim3.getTotalWeight() << std::endl;
std::cout << "Expected longest path: 0->1->2, obtained: " << mstPrim3.getTreeDepthPath() << std::endl;
std::cout << "Expected heaviest edge: 1 2 3, obtained: " << mstPrim3.getMaxWeightEdge() << std::endl;
std::cout << "Expected heaviest path: 0 --(2)--> 1 --(3)--> 2, obtained: " << mstPrim3.getMaxWeightPath() << std::endl;
std::cout << "Expected average distance: 3.33333, obtained: " << mstPrim3.getAverageDistance() << std::endl;
std::cout << "Expected lightest edge: 0 1 2, obtained: " << mstPrim3.getMinWeightEdge() << "\n" << std::endl;

std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;

// --- Test graph with 5 vertices ---
Graph graph5(5);
graph5.addEdge(0, 1, 2);
graph5.addEdge(1, 2, 3);
graph5.addEdge(0, 3, 6);
graph5.addEdge(1, 4, 5);
graph5.addEdge(3, 1, 8);
graph5.addEdge(4, 2, 7);

Graph expectedMST5(5);
expectedMST5.addEdge(0, 1, 2);
expectedMST5.addEdge(1, 2, 3);
expectedMST5.addEdge(0, 3, 6);
expectedMST5.addEdge(1, 4, 5);

std::cout << "--- Tests for graph5 ---" << std::endl;

MST mstPrim5(graph5, "PRIM");
MST mstKruskal5(graph5, "KRUSKAL");
MST mstBoruvka5(graph5, "BORUVKA");
MST mstTarjan5(graph5, "TARJAN");
MST mstInteger5(graph5, "INTEGER_MST");

if (mstPrim5.compareGraphs(expectedMST5)) std::cout << "PRIM: Success" << std::endl; else std::cout << "PRIM: Failure" << std::endl;
if (mstKruskal5.compareGraphs(expectedMST5)) std::cout << "KRUSKAL: Success" << std::endl; else std::cout << "KRUSKAL: Failure" << std::endl;
if (mstBoruvka5.compareGraphs(expectedMST5)) std::cout << "BORUVKA: Success" << std::endl; else std::cout << "BORUVKA: Failure" << std::endl;
if (mstTarjan5.compareGraphs(expectedMST5)) std::cout << "TARJAN: Success" << std::endl; else std::cout << "TARJAN: Failure" << std::endl;
if (mstInteger5.compareGraphs(expectedMST5)) std::cout << "INTEGER_MST: Success" << std::endl; else std::cout << "INTEGER_MST: Failure" << std::endl;

std::cout << mstPrim5.displayGraph() << std::endl;

// Property tests for graph5
std::cout << "Expected total weight: 16, obtained: " << mstPrim5.getTotalWeight() << std::endl;
std::cout << "Expected longest path: 0->1->2, obtained: " << mstPrim5.getTreeDepthPath() << std::endl;
std::cout << "Expected heaviest edge: 0 3 6, obtained: " << mstPrim5.getMaxWeightEdge() << std::endl;
std::cout << "Expected heaviest path: 4 --(5)--> 1 --(2)--> 0 --(6)--> 3, obtained: " << mstPrim5.getMaxWeightPath() << std::endl;
std::cout << "Expected average distance: 6.8, obtained: " << mstPrim5.getAverageDistance() << std::endl;
std::cout << "Expected lightest edge: 0 1 2, obtained: " << mstPrim5.getMinWeightEdge() << "\n" << std::endl;

std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;

// --- Test graph with 6 vertices ---
Graph graph6(6);
graph6.addEdge(0, 1, 6);
graph6.addEdge(1, 3, 2);
graph6.addEdge(3, 5, 8);
graph6.addEdge(5, 4, 8);
graph6.addEdge(4, 0, 9);
graph6.addEdge(2, 0, 3);
graph6.addEdge(2, 1, 4);
graph6.addEdge(2, 3, 2);
graph6.addEdge(2, 5, 9);
graph6.addEdge(2, 4, 9);

Graph expectedMST6(6);
expectedMST6.addEdge(3, 1, 2);
expectedMST6.addEdge(3, 2, 2);
expectedMST6.addEdge(3, 5, 8);
expectedMST6.addEdge(5, 4, 8);
expectedMST6.addEdge(2, 0, 3);

std::cout << "--- Tests for graph6 ---" << std::endl;

MST mstPrim6(graph6, "PRIM");
MST mstKruskal6(graph6, "KRUSKAL");
MST mstBoruvka6(graph6, "BORUVKA");
MST mstTarjan6(graph6, "TARJAN");
MST mstInteger6(graph6, "INTEGER_MST");

if (mstPrim6.compareGraphs(expectedMST6)) std::cout << "PRIM: Success" << std::endl; else std::cout << "PRIM: Failure" << std::endl;
if (mstKruskal6.compareGraphs(expectedMST6)) std::cout << "KRUSKAL: Success" << std::endl; else std::cout << "KRUSKAL: Failure" << std::endl;
if (mstBoruvka6.compareGraphs(expectedMST6)) std::cout << "BORUVKA: Success" << std::endl; else std::cout << "BORUVKA: Failure" << std::endl;
if (mstTarjan6.compareGraphs(expectedMST6)) std::cout << "TARJAN: Success" << std::endl; else std::cout << "TARJAN: Failure" << std::endl;
if (mstInteger6.compareGraphs(expectedMST6)) std::cout << "INTEGER_MST: Success" << std::endl; else std::cout << "INTEGER_MST: Failure" << std::endl;

std::cout << mstPrim6.displayGraph() << std::endl;

// Property tests for graph6
std::cout << "Expected total weight: 23, obtained: " << mstPrim6.getTotalWeight() << std::endl;
std::cout << "Expected longest path: 0->2->3->5->4, obtained: " << mstPrim6.getTreeDepthPath() << std::endl;
std::cout << "Expected heaviest edge: 3 5 8, obtained: " << mstPrim6.getMaxWeightEdge() << std::endl;
std::cout << "Expected heaviest path: 4 --(8)--> 5 --(8)--> 3 --(2)--> 2 --(3)--> 0, obtained: " << mstPrim6.getMaxWeightPath() << std::endl;
std::cout << "Expected average distance: 9.66667, obtained: " << mstPrim6.getAverageDistance() << std::endl;
std::cout << "Expected lightest edge: 1 3 2, obtained: " << mstPrim6.getMinWeightEdge() << "\n" << std::endl;

std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;

{
    // Test with graph expectedMST3
    std::cout << "\nTest on graph expectedMST3:" << std::endl;
    Graph expectedMST3(3);
    expectedMST3.addEdge(0, 1, 2);
    expectedMST3.addEdge(1, 2, 3);

    MST mst3(expectedMST3, "KRUSKAL"); // Create MST using Kruskal's algorithm

    std::cout << mst3.displayGraph() << std::endl;

    // Attempt to add an edge that should succeed
    std::cout << "Adding edge (0, 2, 1) to MST3: "
              << (mst3.add(0, 2, 1) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst3.displayGraph() << std::endl;

    // Attempt to add an edge that should fail
    std::cout << "Adding edge (0, 2, 4) to MST3: "
              << (mst3.add(0, 2, 4) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst3.displayGraph() << std::endl;

    // Attempt to remove an edge that should succeed
    std::cout << "Removing edge (0, 1) from MST3: "
              << (mst3.remove(0, 1) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst3.displayGraph() << std::endl;

    // Attempt to remove an edge that should fail
    std::cout << "Removing edge (1, 2) from MST3: "
              << (mst3.remove(1, 2) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst3.displayGraph() << std::endl;

    std::cout << std::endl;

    // Test with graph expectedMST5
    std::cout << "Test on graph expectedMST5:" << std::endl;
    Graph expectedMST5(5);
    expectedMST5.addEdge(0, 1, 2);
    expectedMST5.addEdge(1, 2, 3);
    expectedMST5.addEdge(0, 3, 6);
    expectedMST5.addEdge(1, 4, 5);

    MST mst5(expectedMST5, "PRIM"); // Create MST using Prim's algorithm

    std::cout << "Adding edge (3, 4, 4) to MST5: "
              << (mst5.add(3, 4, 4) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst5.displayGraph() << std::endl;

    std::cout << "Adding edge (2, 3, 7) to MST5: "
              << (mst5.add(2, 3, 7) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst5.displayGraph() << std::endl;

    std::cout << "Removing edge (1, 2) from MST5: "
              << (mst5.remove(1, 2) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst5.displayGraph() << std::endl;

    std::cout << "Removing edge (0, 3) from MST5: "
              << (mst5.remove(0, 3) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst5.displayGraph() << std::endl;

    std::cout << std::endl;

    // Test with graph expectedMST6
    std::cout << "Test on graph expectedMST6:" << std::endl;
    Graph expectedMST6(6);
    expectedMST6.addEdge(3, 1, 2);
    expectedMST6.addEdge(3, 2, 2);
    expectedMST6.addEdge(3, 5, 8);
    expectedMST6.addEdge(5, 4, 8);
    expectedMST6.addEdge(2, 0, 3);

    MST mst6(expectedMST6, "BORUVKA"); // Create MST using Boruvka's algorithm

    std::cout << "Adding edge (1, 4, 6) to MST6: "
              << (mst6.add(1, 4, 6) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst6.displayGraph() << std::endl;

    std::cout << "Adding edge (2, 5, 10) to MST6: "
              << (mst6.add(2, 5, 10) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst6.displayGraph() << std::endl;

    std::cout << "Removing edge (3, 2) from MST6: "
              << (mst6.remove(3, 2) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst6.displayGraph() << std::endl;

    std::cout << "Removing edge (5, 4) from MST6: "
              << (mst6.remove(5, 4) ? "Succeeded" : "Failed") << std::endl;
    std::cout << mst6.displayGraph() << std::endl;
}

}