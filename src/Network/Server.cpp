#include "Server.hpp"

#include <iostream>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <set>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <memory>
#include <tuple>
#include "../../src/Model/Graph.hpp"
#include "../../src/Model/MSTFactory.hpp"
#include "../../src/Network/Pipeline.hpp"
#include "../../src/Network/ActiveObject.hpp"

// Constructeur
Server::Server() : serverRunning(true), activeClients(0), serverFd(-1) {
    std::cout << "Server instance created." << std::endl;
}

// Destructeur pour libérer les ressources
Server::~Server() {
    if (serverFd != -1) {
        shutdown(serverFd, SHUT_RDWR);
        close(serverFd);
    }
    closeAllClients();
    std::cout << "Server instance destroyed and resources freed." << std::endl;
}

// Main server function using the Leader-Follower pattern
void Server::runServer(){
    struct sockaddr_in address;       // Structure for socket address
    int opt = 1;                      // Option for setting socket options
    int addrlen = sizeof(address);    // Length of the address
    int newSocket;                    // Socket for new client connections

    // Create an ActiveObject with a thread pool of THREAD_POOL_SIZE
    ActiveObject activeObject(THREAD_POOL_SIZE);

    // Create the server socket
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed"); // Print error if socket creation fails
        exit(EXIT_FAILURE);
    }
    // Set socket options
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt"); // Print error if setting options fails
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections on any IP address
    address.sin_port = htons(PORT); // Set the port for the server
    // Bind the socket to the port
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Bind failed"); // Print error if binding fails
        exit(EXIT_FAILURE);
    }
    // Start listening for client connections
    if (listen(serverFd, 10) < 0) {
        perror("Listen failed"); // Print error if listening fails
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is running and listening on port " << PORT << std::endl; // Print server start message

    // Create a thread pool for handling client requests
    std::vector<std::thread> threadPool;
    for (int i = 0; i < THREAD_POOL_SIZE; ++i){
        threadPool.emplace_back([&](){
            // While the server is running
            while (serverRunning) {
                int clientSocket;

                // Leader-Follower mechanism: One thread acts as the leader and processes new connections
                {
                    std::unique_lock<std::mutex> lock(leaderMutex);
                    leaderCV.wait(lock, [&]() { return !clientQueue.empty() || !serverRunning; }); // Wait for new connections
                    // If the server is shutting down, exit the loop
                    if (!serverRunning) {return;}
                    // If there are client connections waiting
                    if (!clientQueue.empty()) {
                        clientSocket = clientQueue.front(); // Get the next client socket
                        clientQueue.pop(); // Remove it from the queue
                    }
                }

                // Process client requests asynchronously using the ActiveObject pattern
                activeObject.enqueueTask([this, clientSocket]() {
                    handleClient(clientSocket); // Handle the client in a separate task
                });
            }
        });
    }

    // Thread to listen for a shutdown command from the server console
    std::thread shutdownThread([&](){
        std::string input;
        // While the server is running
        while (serverRunning) {
            std::cin >> input; // Wait for user input in the console
            // If the input is "shutdown"
            if (input == "shutdown") {
                std::cout << "Server shutting down...\n"; // Print shutdown message
                serverRunning = false;  // Set the server running flag to false
                // Shut down the server socket to stop accepting new connections
                shutdown(serverFd, SHUT_RDWR); // Disable read/write operations on the server socket
                close(serverFd);  // Close the server socket
                closeAllClients(); // Close all active client connections
                leaderCV.notify_all();  // Wake up all waiting threads to stop
                break;
            }
        }
    });

    // Main loop to accept new client connections
    while (serverRunning){
        newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen); // Accept a new client connection
        // If a new client is connected
        if (newSocket >= 0) {
            std::cout << "New client connection accepted.\n"; {
                std::lock_guard<std::mutex> lock(leaderMutex);
                clientQueue.push(newSocket); // Add the client socket to the queue
            }
            leaderCV.notify_one(); // Notify a thread to handle this client
        }
         // If the server is shutting down. Exit the loop
        else if (!serverRunning){break; }
    }

    // Wait for the shutdown thread to finish
    shutdownThread.join();

    // Join all threads to ensure they finish gracefully
    for (auto &th : threadPool){
        // If the thread can be joined. Wait for it to finish
        if (th.joinable()) {th.join();}
    }

    std::cout << "Server has shut down immediately.\n"; // Print shutdown message
}

// Function to close all active client connections
void Server::closeAllClients(){
    std::lock_guard<std::mutex> lock(clientSocketMutex); // Lock the mutex to ensure thread safety
    // Iterate through all active client sockets
    for (int clientSocket : activeClientSockets) {
        shutdown(clientSocket, SHUT_RDWR); // Disable both reading and writing on the client socket
        close(clientSocket); // Close the socket
        std::cout << "Closed client socket: " << clientSocket << std::endl; // Output a message about the closed client socket
    }
    activeClientSockets.clear(); // Clear the set of active clients
}

std::tuple<int, int, std::vector<std::tuple<int, int, int>>, std::string> Server::requestGraphParametersFromClient(int newSocket) {
    char buffer[1024] = {0};
    int numVertices = 0, numEdges = 0;
    std::string response;
    std::vector<std::tuple<int, int, int>> edges;

    // Demande le nombre de sommets
    while (true) {
        response = "Enter the number of vertices:\n";
        ::send(newSocket, response.c_str(), response.size(), 0);

        int valread = ::read(newSocket, buffer, 1024);
        if (valread <= 0) {
            std::cout << "Client disconnected.\n";
            throw std::runtime_error("Client disconnected while entering number of vertices.");
        }

        std::string request(buffer);
        std::istringstream iss(request);
        if (!(iss >> numVertices) || numVertices <= 0) {
            std::string errorResponse = "Invalid input. Please provide a positive integer for the number of vertices.\n";
            ::send(newSocket, errorResponse.c_str(), errorResponse.size(), 0);
        } else {
            response = "Graph will have " + std::to_string(numVertices) + " vertices.\n";
            ::send(newSocket, response.c_str(), response.size(), 0);
            break;
        }
        memset(buffer, 0, sizeof(buffer));  // Réinitialise le buffer
    }

    // Demande le nombre d'arêtes
    while (true) {
        response = "Enter the number of edges:\n";
        ::send(newSocket, response.c_str(), response.size(), 0);

        int valread = ::read(newSocket, buffer, 1024);
        if (valread <= 0) {
            std::cout << "Client disconnected.\n";
            throw std::runtime_error("Client disconnected while entering number of edges.");
        }

        std::string edgesRequest(buffer);
        std::istringstream edgesIss(edgesRequest);
        if (!(edgesIss >> numEdges) || numEdges < 0) {
            std::string errorResponse = "Invalid input. Please provide a non-negative integer for the number of edges.\n";
            ::send(newSocket, errorResponse.c_str(), errorResponse.size(), 0);
        } else {
            response = "You have chosen to add " + std::to_string(numEdges) + " edges.\n";
            ::send(newSocket, response.c_str(), response.size(), 0);
            break;
        }
        memset(buffer, 0, sizeof(buffer));  // Réinitialise le buffer
    }

    // Demande les détails des arêtes
    for (int i = 0; i < numEdges; ++i) {
        while (true) {
            std::string edgePrompt = "Enter edge " + std::to_string(i + 1) + " in format <u> <v> <weight> (non-negative integers):\n";
            ::send(newSocket, edgePrompt.c_str(), edgePrompt.size(), 0);

            int valread = ::read(newSocket, buffer, 1024);
            if (valread <= 0) {
                std::cout << "Client disconnected.\n";
                throw std::runtime_error("Client disconnected while entering edge data.");
            }

            std::string edgeInput(buffer);
            std::istringstream edgeIss(edgeInput);
            int u = 0, v = 0, weight = 0;

            if (!(edgeIss >> u >> v >> weight) || u < 0 || v < 0 || weight < 0 || u >= numVertices || v >= numVertices) {
                std::string errorResponse = "Invalid edge input. Ensure vertices are between 0 and " + std::to_string(numVertices - 1) + " and weight is non-negative.\n";
                ::send(newSocket, errorResponse.c_str(), errorResponse.size(), 0);
            } else {
                edges.emplace_back(u, v, weight);
                response = "Edge " + std::to_string(i + 1) + " added between " + std::to_string(u) + " and " + std::to_string(v) + " with weight " + std::to_string(weight) + ".\n";
                ::send(newSocket, response.c_str(), response.size(), 0);
                break;
            }
            memset(buffer, 0, sizeof(buffer));  // Réinitialise le buffer
        }
    }

    // Demande à l'utilisateur de choisir un algorithme pour le MST
    response = "Choose an algorithm to find the Minimum Spanning Tree (MST):\n"
               "PRIM, KRUSKAL, BORUVKA, TARJAN, INTEGER_MST\n"
               "Enter the name of the algorithm: ";
    ::send(newSocket, response.c_str(), response.size(), 0);

    memset(buffer, 0, sizeof(buffer));
    int valread = ::read(newSocket, buffer, 1024);
    if (valread <= 0) {
        std::cout << "Client disconnected.\n";
        throw std::runtime_error("Client disconnected while choosing MST algorithm.");
    }

    std::string choice(buffer);
    choice = choice.substr(0, choice.find_first_of("\r\n"));  // Supprime les nouvelles lignes et espaces blancs
    std::transform(choice.begin(), choice.end(), choice.begin(), ::toupper); // Met en majuscules pour correspondre

    // Retourne tous les paramètres, y compris le choix de l'algorithme
    return {numVertices, numEdges, edges, choice};
}

// Function to handle a single client connection
void Server::handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE] = {0}; // Buffer to store incoming client data

    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Graph> graph;            // Pointer to store the client's graph
    std::unique_ptr<Graph> mst;              // Pointer to store the client's computed MST
    std::string algorithmChoice = "prim";    // Store the chosen algorithm for MST computation
    MSTFactory* algo = nullptr;              // Pointer to MST algorithm factory
    ////////////////////////////////////////////////////////////////////////////////

    // Add the client socket to the set of active clients
    {
        std::lock_guard<std::mutex> lock(clientSocketMutex);
        activeClientSockets.insert(clientSocket);
    }
    activeClients++; // Increment the counter of active clients

    // Display the help menu with available commands
    std::string helpMenu = "------------------ COMMAND MENU ------------------\n";
    helpMenu += "Create a new graph:\n";
    helpMenu += "   - Syntax: 'create <number_of_vertices>'\n";
    helpMenu += "   - Example: 'create 5' to create a graph with 5 vertices.\n";
    helpMenu += "Add an edge:\n";
    helpMenu += "   - Syntax: 'add <u> <v> <w>'\n";
    helpMenu += "   - Example: 'add 1 2 10' to add an edge between vertices 1 and 2 with weight 10.\n";
    helpMenu += "Remove an edge:\n";
    helpMenu += "   - Syntax: 'remove <u> <v>'\n";
    helpMenu += "   - Example: 'remove 1 2' to remove the edge between vertices 1 and 2.\n";
    helpMenu += "Choose MST Algorithm:\n";
    helpMenu += "   - Syntax: 'algorithm <algorithm_name>'\n";
    helpMenu += "   - Available: prim, kruskal, boruvka, tarjan, integer_mst\n";
    helpMenu += "Display MST and Analysis:\n";
    helpMenu += "   - Syntax: 'display'\n";
    helpMenu += "   - Shows the Graph, MST and an analysis summary.\n";
    helpMenu += "Shutdown:\n";
    helpMenu += "   - Syntax: 'shutdown'\n";
    helpMenu += "   - Closes the connection with the server.\n";
    helpMenu += "--------------------------------------------------\n";
    send(clientSocket, helpMenu.c_str(), helpMenu.size(), 0);

    while (serverRunning) {

        int bytesRead = read(clientSocket, buffer, BUFFER_SIZE); // Read data from the client
        if (bytesRead <= 0) { // If no data is read, assume the client disconnected
            std::cout << "Client disconnected.\n";
            break;
        }

        std::string request(buffer, bytesRead);
        std::stringstream ss(request);
        std::string command;
        ss >> command;

        // Create a pipeline to handle multiple steps in sequence
        Pipeline pipeline;

        // Handle the "create" command to create a new graph
        if (command == "create") {
            pipeline.addStep([&]() {
                int size;
                ss >> size; // Read graph size (number of vertices)
                graph = std::make_unique<Graph>(size); // Create a new graph
                std::string response = "Graph created with " + std::to_string(size) + " vertices.\n";
                send(clientSocket, response.c_str(), response.size(), 0); // Send response
            });
        }
        // Handle the "add" command to add an edge to the graph
        else if (command == "add") {
            pipeline.addStep([&]() {
                if (!graph) {
                    std::string response = "Graph not created. Use 'create' first.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                    return;
                }
                int u, v, weight;
                ss >> u >> v >> weight; // Read vertices and edge weight
                graph->add_edge_on_Graph(u, v, weight); // Add edge to the graph
                std::string response = "Edge added: (" + std::to_string(u) + ", " + std::to_string(v) + ") with weight " + std::to_string(weight) + "\n";
                send(clientSocket, response.c_str(), response.size(), 0);
            });
        }
        // Handle the "remove" command to remove an edge from the graph
        else if (command == "remove") {
            pipeline.addStep([&]() {
                if (!graph) {
                    std::string response = "Graph not created. Use 'create' first.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                    return;
                }
                int u, v;
                ss >> u >> v; // Read vertices of edge to remove
                graph->remove_edge_on_Graph(u, v); // Remove edge
                std::string response = "Edge removed: (" + std::to_string(u) + ", " + std::to_string(v) + ")\n";
                send(clientSocket, response.c_str(), response.size(), 0);
            });
        }
        // Handle the "algo" command to set MST algorithm
        else if (command == "algo") {
            pipeline.addStep([&]() {
                std::string selectedAlgorithm;
                ss >> selectedAlgorithm;

                // Check if the algorithm choice is valid before updating algorithmChoice
                if (selectedAlgorithm == "prim" || selectedAlgorithm == "kruskal" ||
                    selectedAlgorithm == "boruvka" || selectedAlgorithm == "tarjan" ||
                    selectedAlgorithm == "integer_mst") {

                    algorithmChoice = selectedAlgorithm; // Update algorithm choice if valid
                    std::string response = "Algorithm set to " + algorithmChoice + ".\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                } else {
                    // Send an error response if the algorithm choice is invalid
                    std::string response = "Error: Unknown algorithm '" + selectedAlgorithm + "'. Available options: prim, kruskal, boruvka, tarjan, integer_mst.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                }
            });
        }
        // Handle the "display" command to compute and display the MST
        else if (command == "display") {
            pipeline.addStep([&]() {
                if (!graph) {
                    std::string response = "Graph not created.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                    return;
                }

                // Set MST algorithm based on the user's choice
                if (algorithmChoice == "prim") algo = new PrimSolver();
                else if (algorithmChoice == "kruskal") algo = new KruskalSolver();
                else if (algorithmChoice == "boruvka") algo = new BoruvkaSolver();
                else if (algorithmChoice == "tarjan") algo = new TarjanSolver();
                else if (algorithmChoice == "integer_mst") algo = new IntegerMSTSolver();

                if (algo) {
                    mst = std::make_unique<Graph>(algo->solveMST(*graph)); // Compute MST
                    delete algo;
                } else {
                    std::string response = "Unknown algorithm.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                    return;
                }

                std::string response = graph->displayGraph();
                response += mst->displayGraph();

                // MST analysis metrics
                int totalWeight = mst->getTotalWeight();
                std::string longestPath = mst->getTreeDepthPath_MST();
                std::string heaviestEdge = mst->getMaxWeightEdge_MST();
                std::string heaviestPath = mst->getMaxWeightPath_MST();
                std::string lightestEdge = mst->getMinWeightEdge_MST();
                double averageDistance = mst->getAverageDistance_MST();

                response += "- - - MST Analysis - - -\n";
                response += "Algorithm: " + algorithmChoice + "\n";
                response += "Total MST weight: " + std::to_string(totalWeight) + "\n";
                response += "Longest path: " + longestPath + "\n";
                response += "Heaviest path: " + heaviestPath + "\n";
                response += "Average distance: " + std::to_string(averageDistance) + "\n";
                response += "Heaviest edge: " + heaviestEdge + "\n";
                response += "Lightest edge: " + lightestEdge + "\n";

                send(clientSocket, response.c_str(), response.size(), 0);
            });
        }
        // Handle the "shutdown" command to terminate client connection
        else if (command == "shutdown") {
            pipeline.addStep([&]() {
                std::string response = "Shutting down client.\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                std::cout << "Client initiated shutdown.\n";

                close(clientSocket); // Close client socket
                {
                    std::lock_guard<std::mutex> lock(clientSocketMutex);
                    activeClientSockets.erase(clientSocket); // Remove from active set
                }
                activeClients--; // Decrement active client count
                return; // Exit the loop for this client
            });
        }
        // Handle unknown command
        else {
            pipeline.addStep([&]() {
                std::string response = "Unknown command.\n";
                send(clientSocket, response.c_str(), response.size(), 0);
            });
        }

        // Execute all steps in the pipeline for this command
        pipeline.execute();
    }

    // Ensure client is removed from active set and socket is closed if not done earlier
    {
        std::lock_guard<std::mutex> lock(clientSocketMutex);
        activeClientSockets.erase(clientSocket); // Remove from active set
    }
    close(clientSocket); // Close client socket
    std::cout << "Client socket closed.\n"; // Log socket closure
}