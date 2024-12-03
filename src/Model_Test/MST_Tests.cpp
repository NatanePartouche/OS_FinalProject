#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../../src/Model_Test/doctest.h"
#include "../../src/Model/Graph.hpp"
#include "../../src/Model/MSTFactory.hpp"

MSTFactory* solverPrim = new PrimSolver();
MSTFactory* solverKruskal = new KruskalSolver();
MSTFactory* solverBoruvka = new BoruvkaSolver();
MSTFactory* solverTarjan = new TarjanSolver();
MSTFactory* solverIntegerMST = new IntegerMSTSolver();

TEST_CASE("Graph: Constructor and Initial State") {
    Graph g(5);
    CHECK(g.getNumVertices() == 5);
    CHECK(g.getTotalWeight() == 0);
    CHECK(g.getAdjList().size() == 5);
}

TEST_CASE("Graph: Adding Edges") {
    Graph g(4);

    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 5);
    g.add_edge(1, 2, 7);

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

TEST_CASE("Graph: Removing Edges") {
    Graph g(4);

    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 5);
    g.add_edge(1, 2, 7);

    g.remove_edge(0, 1);

    // Check the total weight after removal
    CHECK(g.getTotalWeight() == 12);  // 5 + 7

    // Check that the edge 0-1 is removed
    CHECK(g.getAdjList()[0].size() == 1);
    CHECK(g.getAdjList()[1].size() == 1);
}

TEST_CASE("Graph: Changing Edge Weights") {
    Graph g(4);

    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 5);
    g.add_edge(1, 2, 7);

    // Modify the weight of the edge between 0 and 2
    g.changeEdgeWeight(0, 2, 15);

    CHECK(g.getTotalWeight() == 32);  // 10 + 15 + 7

    // Check the new weight in the adjacency list
    CHECK(g.getAdjList()[0].back().second == 15);
}

TEST_CASE("Graph: Vertex Validity") {
    Graph g(5);
    CHECK(g.isValidVertex(0) == true);
    CHECK(g.isValidVertex(4) == true);
    CHECK(g.isValidVertex(5) == false);
    CHECK(g.isValidVertex(-1) == false);
}

TEST_CASE("Graph: Recalculate Total Weight") {
    Graph g(3);

    g.add_edge(0, 1, 4);
    g.add_edge(1, 2, 6);

    CHECK(g.getTotalWeight() == 10);

    // Change the weight of the edge between 0 and 1
    g.changeEdgeWeight(0, 1, 8);

    CHECK(g.getTotalWeight() == 14);

    // Remove an edge and check the total weight
    g.remove_edge(1, 2);
    CHECK(g.getTotalWeight() == 8);
}

TEST_CASE("Graph: Empty Graph") {
    Graph g(0);  // Graph with 0 vertices
    CHECK(g.getNumVertices() == 0);
    CHECK(g.getTotalWeight() == 0);

    // Trying to add_edge an edge to a graph with no vertices should do nothing
    g.add_edge(0, 1, 10);
    CHECK(g.getTotalWeight() == 0);
    CHECK(g.getAdjList().size() == 0);  // No vertices to add_edge edges to
}

TEST_CASE("Graph: Invalid Operations") {
    Graph g(3);

    // Attempt to add_edge/remove_edge/change edges with invalid vertex indices
    g.add_edge(0, 3, 10);  // Vertex 3 does not exist
    g.add_edge(-1, 2, 5);  // Negative index
    CHECK(g.getTotalWeight() == 0);  // No edges should have been added

    g.remove_edge(0, 3);  // Invalid removal
    g.changeEdgeWeight(0, 3, 20);  // Invalid weight change
    CHECK(g.getTotalWeight() == 0);  // Total weight should remain 0
}

TEST_CASE("Test MST with an undirected graph of 3 vertices") {
    // Create a test graph
    Graph graph(3);
    graph.add_edge(0, 1, 2);  // 0 - 1
    graph.add_edge(1, 2, 3);  // 1 - 2
    graph.add_edge(0, 2, 10); // 0 - 2

    // Expected Minimum Spanning Tree
    Graph expectedMST(3);
    expectedMST.add_edge(0, 1, 2);  // 0 - 1
    expectedMST.add_edge(1, 2, 3);  // 1 - 2

    Graph mstPrim = solverPrim->solveMST(graph);
    CHECK(mstPrim.compareGraphs(expectedMST));

    Graph mstKruskal = solverKruskal->solveMST(graph);
    CHECK(mstKruskal.compareGraphs(expectedMST));

    Graph mstBoruvka = solverBoruvka->solveMST(graph);
    CHECK(mstBoruvka.compareGraphs(expectedMST));

    Graph mstTarjan = solverTarjan->solveMST(graph);
    CHECK(mstTarjan.compareGraphs(expectedMST));

    Graph mstInteger = solverIntegerMST->solveMST(graph);
    CHECK(mstInteger.compareGraphs(expectedMST));
}

TEST_CASE("Test MST with an undirected graph of 5 vertices") {
    Graph graph(5);
    graph.add_edge(0, 1, 2);
    graph.add_edge(1, 2, 3);
    graph.add_edge(0, 3, 6);
    graph.add_edge(1, 4, 5);
    graph.add_edge(3, 1, 8);
    graph.add_edge(4, 2, 7);

    Graph expectedMST(5);
    expectedMST.add_edge(0, 1, 2);
    expectedMST.add_edge(1, 2, 3);
    expectedMST.add_edge(0, 3, 6);
    expectedMST.add_edge(1, 4, 5);

    Graph mstPrim = solverPrim->solveMST(graph);
    CHECK(mstPrim.compareGraphs(expectedMST));

    Graph mstKruskal = solverKruskal->solveMST(graph);
    CHECK(mstKruskal.compareGraphs(expectedMST));

    Graph mstBoruvka = solverBoruvka->solveMST(graph);
    CHECK(mstBoruvka.compareGraphs(expectedMST));

    Graph mstTarjan = solverTarjan->solveMST(graph);
    CHECK(mstTarjan.compareGraphs(expectedMST));

    Graph mstInteger = solverIntegerMST->solveMST(graph);
    CHECK(mstInteger.compareGraphs(expectedMST));
}

TEST_CASE("Test MST with an undirected graph of 6 vertices") {
    Graph graph(6);
    graph.add_edge(0, 1, 6);
    graph.add_edge(1, 3, 2);
    graph.add_edge(3, 5, 8);
    graph.add_edge(5, 4, 8);
    graph.add_edge(4, 0, 9);
    graph.add_edge(2, 0, 3);
    graph.add_edge(2, 1, 4);
    graph.add_edge(2, 3, 2);
    graph.add_edge(2, 5, 9);
    graph.add_edge(2, 4, 9);

    Graph expectedMST(6);
    expectedMST.add_edge(3, 1, 2);
    expectedMST.add_edge(3, 2, 2);
    expectedMST.add_edge(3, 5, 8);
    expectedMST.add_edge(5, 4, 8);
    expectedMST.add_edge(2, 0, 3);

    Graph mstPrim = solverPrim->solveMST(graph);
    CHECK(mstPrim.compareGraphs(expectedMST));

    Graph mstKruskal = solverKruskal->solveMST(graph);
    CHECK(mstKruskal.compareGraphs(expectedMST));

    Graph mstBoruvka = solverBoruvka->solveMST(graph);
    CHECK(mstBoruvka.compareGraphs(expectedMST));

    Graph mstTarjan = solverTarjan->solveMST(graph);
    CHECK(mstTarjan.compareGraphs(expectedMST));

    Graph mstInteger = solverIntegerMST->solveMST(graph);
    CHECK(mstInteger.compareGraphs(expectedMST));
}

TEST_CASE("Test MST with a disconnected graph of 6 vertices") {
    Graph disconnectedGraph(6);
    disconnectedGraph.add_edge(0, 1, 6);
    disconnectedGraph.add_edge(2, 3, 2);
    disconnectedGraph.add_edge(4, 5, 8);

    Graph expectedEmptyGraph(0);

    Graph mstPrim = solverPrim->solveMST(disconnectedGraph);
    CHECK(mstPrim.compareGraphs(expectedEmptyGraph));

    Graph mstKruskal = solverKruskal->solveMST(disconnectedGraph);
    CHECK(mstKruskal.compareGraphs(expectedEmptyGraph));

    Graph mstBoruvka = solverBoruvka->solveMST(disconnectedGraph);
    CHECK(mstBoruvka.compareGraphs(expectedEmptyGraph));

    Graph mstTarjan = solverTarjan->solveMST(disconnectedGraph);
    CHECK(mstTarjan.compareGraphs(expectedEmptyGraph));

    Graph mstInteger = solverIntegerMST->solveMST(disconnectedGraph);
    CHECK(mstInteger.compareGraphs(expectedEmptyGraph));
}

TEST_CASE("MST: Total Weight Calculation") {
    // Create a simple graph
    Graph g(4);
    g.add_edge(0, 1, 10);
    g.add_edge(0, 2, 5);
    g.add_edge(1, 2, 7);
    g.add_edge(2, 3, 3);

    Graph mstPrim = solverPrim->solveMST(g);
    // Verify the total weight of the MST using Prim's algorithm
    CHECK(mstPrim.getTotalWeight() == 15);  // Edges selected: (0-2), (2-3), (1-2)

    Graph mstKruskal = solverKruskal->solveMST(g);
    // Verify the total weight of the MST using Kruskal's algorithm
    CHECK(mstKruskal.getTotalWeight() == 15);  // Edges selected: (0-2), (2-3), (1-2)

    Graph mstBoruvka = solverBoruvka->solveMST(g);
    // Verify the total weight of the MST using Kruskal's algorithm
    CHECK(mstBoruvka.getTotalWeight() == 15);  // Edges selected: (0-2), (2-3), (1-2)

    Graph mstTarjan = solverTarjan->solveMST(g);
    // Verify the total weight of the MST using Kruskal's algorithm
    CHECK(mstTarjan.getTotalWeight() == 15);  // Edges selected: (0-2), (2-3), (1-2)

    Graph mstInteger = solverIntegerMST->solveMST(g);
    // Verify the total weight of the MST using Kruskal's algorithm
    CHECK(mstInteger.getTotalWeight() == 15);  // Edges selected: (0-2), (2-3), (1-2)

}

// TEST_CASE("BIG TESTS") {
// // --- Test graph with 3 vertices ---
// Graph graph3(3);
// graph3.add_edge(0, 1, 2);
// graph3.add_edge(1, 2, 3);
// graph3.add_edge(0, 2, 10);
//
// Graph expectedMST3(3);
// expectedMST3.add_edge(0, 1, 2);
// expectedMST3.add_edge(1, 2, 3);
//
// std::cout << "\n//////////////////////////////////////////////////////////////////////////" << std::endl;
// std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
// std::cout << "--- Tests for graph3 ---" << std::endl;
//
//
//     Graph mstPrim3 = solverPrim->solveMST(graph3);
//     Graph mstKruskal3 = solverKruskal->solveMST(graph3);
//     Graph mstBoruvka3 = solverBoruvka->solveMST(graph3);
//     Graph mstTarjan3 = solverTarjan->solveMST(graph3);
//     Graph mstInteger3 = solverIntegerMST->solveMST(graph3);
//
// if (mstPrim3.compareGraphs(expectedMST3)) std::cout << "PRIM: Success" << std::endl; else std::cout << "PRIM: Failure" << std::endl;
// if (mstKruskal3.compareGraphs(expectedMST3)) std::cout << "KRUSKAL: Success" << std::endl; else std::cout << "KRUSKAL: Failure" << std::endl;
// if (mstBoruvka3.compareGraphs(expectedMST3)) std::cout << "BORUVKA: Success" << std::endl; else std::cout << "BORUVKA: Failure" << std::endl;
// if (mstTarjan3.compareGraphs(expectedMST3)) std::cout << "TARJAN: Success" << std::endl; else std::cout << "TARJAN: Failure" << std::endl;
// if (mstInteger3.compareGraphs(expectedMST3)) std::cout << "INTEGER_MST: Success" << std::endl; else std::cout << "INTEGER_MST: Failure" << std::endl;
//
// std::cout << mstPrim3.displayGraph() << std::endl;
//
// // Property tests for graph3
// std::cout << "Expected total weight: 5, obtained: " << mstPrim3.getTotalWeight() << std::endl;
// std::cout << "Expected longest path: 0->1->2, obtained: " << mstPrim3.getTreeDepthPath_MST() << std::endl;
// std::cout << "Expected heaviest edge: 1 2 3, obtained: " << mstPrim3.getMaxWeightEdge_MST() << std::endl;
// std::cout << "Expected heaviest path:  2 --(3)--> 1 --(2)--> 0, obtained: " << mstPrim3.getMaxWeightPath_MST() << std::endl;
// std::cout << "Expected average distance: 3.33333, obtained: " << mstPrim3.getAverageDistance_MST() << std::endl;
// std::cout << "Expected lightest edge: 0 1 2, obtained: " << mstPrim3.getMinWeightEdge_MST() << "\n" << std::endl;
//
// std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
// std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
//
// // --- Test graph with 5 vertices ---
// Graph graph5(5);
// graph5.add_edge(0, 1, 2);
// graph5.add_edge(1, 2, 3);
// graph5.add_edge(0, 3, 6);
// graph5.add_edge(1, 4, 5);
// graph5.add_edge(3, 1, 8);
// graph5.add_edge(4, 2, 7);
//
// Graph expectedMST5(5);
// expectedMST5.add_edge(0, 1, 2);
// expectedMST5.add_edge(1, 2, 3);
// expectedMST5.add_edge(0, 3, 6);
// expectedMST5.add_edge(1, 4, 5);
//
// std::cout << "--- Tests for graph5 ---" << std::endl;
//
//     Graph mstPrim5 = solverPrim->solveMST(graph5);
//     Graph mstKruskal5 = solverKruskal->solveMST(graph5);
//     Graph mstBoruvka5 = solverBoruvka->solveMST(graph5);
//     Graph mstTarjan5 = solverTarjan->solveMST(graph5);
//     Graph mstInteger5 = solverIntegerMST->solveMST(graph5);
//
// if (mstPrim5.compareGraphs(expectedMST5)) std::cout << "PRIM: Success" << std::endl; else std::cout << "PRIM: Failure" << std::endl;
// if (mstKruskal5.compareGraphs(expectedMST5)) std::cout << "KRUSKAL: Success" << std::endl; else std::cout << "KRUSKAL: Failure" << std::endl;
// if (mstBoruvka5.compareGraphs(expectedMST5)) std::cout << "BORUVKA: Success" << std::endl; else std::cout << "BORUVKA: Failure" << std::endl;
// if (mstTarjan5.compareGraphs(expectedMST5)) std::cout << "TARJAN: Success" << std::endl; else std::cout << "TARJAN: Failure" << std::endl;
// if (mstInteger5.compareGraphs(expectedMST5)) std::cout << "INTEGER_MST: Success" << std::endl; else std::cout << "INTEGER_MST: Failure" << std::endl;
//
// std::cout << mstPrim5.displayGraph() << std::endl;
//
// // Property tests for graph5
// std::cout << "Expected total weight: 16, obtained: " << mstPrim5.getTotalWeight() << std::endl;
// std::cout << "Expected longest path: 0->1->2, obtained: " << mstPrim5.getTreeDepthPath_MST() << std::endl;
// std::cout << "Expected heaviest edge: 0 3 6, obtained: " << mstPrim5.getMaxWeightEdge_MST() << std::endl;
// std::cout << "Expected heaviest path: 4 --(5)--> 1 --(2)--> 0 --(6)--> 3, obtained: " << mstPrim5.getMaxWeightPath_MST() << std::endl;
// std::cout << "Expected average distance: 6.8, obtained: " << mstPrim5.getAverageDistance_MST() << std::endl;
// std::cout << "Expected lightest edge: 0 1 2, obtained: " << mstPrim5.getMinWeightEdge_MST() << "\n" << std::endl;
//
// std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
// std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
//
// // --- Test graph with 6 vertices ---
// Graph graph6(6);
// graph6.add_edge(0, 1, 6);
// graph6.add_edge(1, 3, 2);
// graph6.add_edge(3, 5, 8);
// graph6.add_edge(5, 4, 8);
// graph6.add_edge(4, 0, 9);
// graph6.add_edge(2, 0, 3);
// graph6.add_edge(2, 1, 4);
// graph6.add_edge(2, 3, 2);
// graph6.add_edge(2, 5, 9);
// graph6.add_edge(2, 4, 9);
//
// Graph expectedMST6(6);
// expectedMST6.add_edge(3, 1, 2);
// expectedMST6.add_edge(3, 2, 2);
// expectedMST6.add_edge(3, 5, 8);
// expectedMST6.add_edge(5, 4, 8);
// expectedMST6.add_edge(2, 0, 3);
//
// std::cout << "--- Tests for graph6 ---" << std::endl;
//
//     Graph mstPrim6 = solverPrim->solveMST(graph6);
//     Graph mstKruskal6 = solverKruskal->solveMST(graph6);
//     Graph mstBoruvka6 = solverBoruvka->solveMST(graph6);
//     Graph mstTarjan6 = solverTarjan->solveMST(graph6);
//     Graph mstInteger6 = solverIntegerMST->solveMST(graph6);
//
// if (mstPrim6.compareGraphs(expectedMST6)) std::cout << "PRIM: Success" << std::endl; else std::cout << "PRIM: Failure" << std::endl;
// if (mstKruskal6.compareGraphs(expectedMST6)) std::cout << "KRUSKAL: Success" << std::endl; else std::cout << "KRUSKAL: Failure" << std::endl;
// if (mstBoruvka6.compareGraphs(expectedMST6)) std::cout << "BORUVKA: Success" << std::endl; else std::cout << "BORUVKA: Failure" << std::endl;
// if (mstTarjan6.compareGraphs(expectedMST6)) std::cout << "TARJAN: Success" << std::endl; else std::cout << "TARJAN: Failure" << std::endl;
// if (mstInteger6.compareGraphs(expectedMST6)) std::cout << "INTEGER_MST: Success" << std::endl; else std::cout << "INTEGER_MST: Failure" << std::endl;
//
// std::cout << mstPrim6.displayGraph() << std::endl;
//
// // Property tests for graph6
// std::cout << "Expected total weight: 23, obtained: " << mstPrim6.getTotalWeight() << std::endl;
// std::cout << "Expected longest path: 0->2->3->5->4, obtained: " << mstPrim6.getTreeDepthPath_MST() << std::endl;
// std::cout << "Expected heaviest edge: 3 5 8, obtained: " << mstPrim6.getMaxWeightEdge_MST() << std::endl;
// std::cout << "Expected heaviest path: 4 --(8)--> 5 --(8)--> 3 --(2)--> 2 --(3)--> 0, obtained: " << mstPrim6.getMaxWeightPath_MST() << std::endl;
// std::cout << "Expected average distance: 9.66667, obtained: " << mstPrim6.getAverageDistance_MST() << std::endl;
// std::cout << "Expected lightest edge: 1 3 2, obtained: " << mstPrim6.getMinWeightEdge_MST() << "\n" << std::endl;
//
// std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
// std::cout << "//////////////////////////////////////////////////////////////////////////" << std::endl;
//
// }