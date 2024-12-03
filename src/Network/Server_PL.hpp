#ifndef SERVER_PL_HPP
#define SERVER_PL_HPP

#include <thread>                   // For creating and managing threads.
#include <sstream>                   // For parsing client input commands.
#include "ActiveObject.hpp"          // ActiveObject for task execution.
#include "../../src/Model/Graph.hpp" // Graph model used for MST operations.

/**
 * @class Server_PL
 * @brief Implements a server using the Pipeline design pattern.
 */
class Server_PL : public Server {
public:
    // Constructor to initialize the server with an address and port.
    Server_PL(const std::string& addr, int port) : Server(addr, port) {
        setupServerSocket(); // Sets up the server socket for communication.
        log("[Server_PL] Server configured on " + address + ":" + std::to_string(port)); // Logs configuration.
    }

    // Starts the server and handles incoming client connections.
    void start() override {
        // Ensures the server isn't started multiple times.
        if (running.exchange(true)) {
            log("[Server_PL] Server is already running.");
            return;
        }

        log("[Server_PL] Server started.");

        // Main loop to handle incoming client connections.
        while (running) {
            sockaddr_in client_addr{}; // Stores client address information.
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len); // Accepts client connections.

            // If accepting a connection fails, log the error and continue.
            if (client_socket < 0) {
                if (running) {
                    log("[Server_PL] Failed to accept connection.");
                }
                continue;
            }

            log("[Server_PL] New client connected: " + std::to_string(client_socket)); // Logs client connection.

            // If the client is not added successfully, close the connection.
            if (!addClient(client_socket)) {
                close(client_socket);
                continue;
            }

            // Handles client communication in a separate thread.
            std::thread(&Server_PL::handleClient, this, client_socket).detach();
        }
    }

    // Handles communication with an individual client.
    void handleClient(int client_socket) override {
        // Déclare un graphe unique pour ce client
        std::shared_ptr<Graph> graph;

        // Help menu to display available commands to the client.
        std::string helpMenu = "------------------------ COMMAND MENU --------------------------------------------\n";
        helpMenu += "Create a new graph:\n   - Syntax: 'create <number_of_vertices>'\n";
        helpMenu += "Add an edge:\n   - Syntax: 'add <u> <v> <w>'\n";
        helpMenu += "Remove an edge:\n   - Syntax: 'remove <u> <v>'\n";
        helpMenu += "Choose MST Algorithm:\n   - Syntax: 'algo <algorithm_name>'\n     (prim/kruskal/tarjan/boruvka/integer_mst)\n" ;
        helpMenu += "Shutdown:\n   - Syntax: 'shutdown'\n";
        helpMenu += "----------------------------------------------------------------------------------\n";

        // Sends the help menu to the client.
        send(client_socket, helpMenu.c_str(), helpMenu.size(), 0);

        char buffer[1024]; // Buffer to store commands from the client.

        // Continuously processes client commands while the server is running.
        while (running) {
            int bytesRead = read(client_socket, buffer, sizeof(buffer)); // Reads data from the client.
            if (bytesRead <= 0) { // Handles client disconnection or errors.
                std::cout << "Client disconnected.\n";
                break;
            }

            std::string request(buffer, bytesRead); // Converts the received data into a string.
            std::stringstream ss(request);         // Parses the client command.
            std::string command;
            ss >> command;                         // Extracts the main command.

            // Handles the 'create' command for initializing a graph.
            if (command == "create") {
                std::string token;
                if (ss >> token) { // Checks if the number of vertices is provided.
                    try {
                        int size = std::stoi(token); // Converts the token to an integer.
                        if (size < 0) {             // Validates the number of vertices.
                            std::string response = "Error: Number of vertices must be greater than or equal to 0.\n";
                            send(client_socket, response.c_str(), response.size(), 0);
                        } else {
                            graph = std::make_shared<Graph>(size); // Creates or resets the graph.
                            std::string response = "Graph created with " + std::to_string(size) + " vertices.\n";
                            send(client_socket, response.c_str(), response.size(), 0);
                        }
                    } catch (const std::invalid_argument&) { // Handles invalid input.
                        std::string response = "Invalid input. Syntax: 'create <number_of_vertices>'\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    } catch (const std::out_of_range&) { // Handles out-of-range values.
                        std::string response = "Error: Number out of range.\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    }
                } else { // Handles missing arguments.
                    std::string response = "Error: Number of vertices not provided. Syntax: 'create <number_of_vertices>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            // Handles the 'add' command to add an edge to the graph.
            else if (command == "add") {
                if (!graph) { // Ensures a graph is created before adding edges.
                    std::string response = "Graph not created. Use 'create' first.\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                    continue;
                }
                int u, v, weight;
                if (ss >> u >> v >> weight) { // Extracts edge details.
                    graph->add_edge(u, v, weight); // Adds the edge to the graph.
                    std::string response = "Edge added: (" + std::to_string(u) + ", " + std::to_string(v) + ") with weight " + std::to_string(weight) + "\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                } else { // Handles invalid syntax.
                    std::string response = "Invalid input. Syntax: 'add <u> <v> <w>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            // Handles the 'remove' command to remove an edge from the graph.
            else if (command == "remove") {
                if (!graph) { // Ensures a graph exists.
                    std::string response = "Graph not created. Use 'create' first.\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                    continue;
                }
                int u, v;
                if (ss >> u >> v) { // Extracts edge details.
                    graph->remove_edge(u, v); // Removes the edge from the graph.
                    std::string response = "Edge removed: (" + std::to_string(u) + ", " + std::to_string(v) + ")\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                } else { // Handles invalid syntax.
                    std::string response = "Invalid input. Syntax: 'remove <u> <v>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            // Handles the 'algo' command to select an MST algorithm.
            else if (command == "algo") {
                if (!graph) { // Ensures a graph exists.
                    log("[Server] Graph not initialized when trying to set algorithm.");
                    std::string response = "Error: Graph not created. Use 'create' first.\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                    continue;
                }

                log("[Server] Received 'algo' command.");
                std::string selectedAlgorithm;
                if (ss >> selectedAlgorithm) { // Extracts the algorithm name.
                    log("[Server] Algorithm selected: " + selectedAlgorithm);
                    if (selectedAlgorithm == "prim" || selectedAlgorithm == "kruskal" ||
                        selectedAlgorithm == "boruvka" || selectedAlgorithm == "tarjan" ||
                        selectedAlgorithm == "integer_mst") {
                        graph->_algorithmChoice = selectedAlgorithm; // Sets the algorithm.
                        log("[Server] Algorithm set successfully.");
                        std::string response = "Algorithm set to " + selectedAlgorithm + ".\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                        } else { // Handles invalid algorithms.
                            log("[Server] Unknown algorithm: " + selectedAlgorithm);
                            std::string response = "Error: Unknown algorithm '" + selectedAlgorithm + "'.\n";
                            send(client_socket, response.c_str(), response.size(), 0);
                        }
                } else { // Handles missing algorithm name.
                    log("[Server] Invalid 'algo' command syntax.");
                    std::string response = "Invalid input. Syntax: 'algo <algorithm_name>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            // Handles the 'shutdown' command to disconnect the client.
            else if (command == "shutdown") {
                std::string response = "Shutting down client.\n";
                send(client_socket, response.c_str(), response.size(), 0);
                break;
            }
            // Handles unknown commands.
            else {
                std::string response = "Unknown command. Use 'help' for a list of commands.\n";
                send(client_socket, response.c_str(), response.size(), 0);
            }

            if(graph){

            graph->Solve();

            // Création des étapes
            ActiveObject step1, step2, step3, step4;

            // Variable pour stocker le résultat final
            std::string finalResult;

            // Étape 1 : Ajout des informations de base du graphe
            step1.enqueue([&]() {
                finalResult += graph->displayGraph();
                finalResult += graph->displayMST();
                finalResult += std::string(15, ' ') + "------------------MST Analysis-------------------------\n";
                finalResult += std::string(15, ' ') +"Algorithm: " + graph->_algorithmChoice + "\n";
                finalResult += std::string(15, ' ') +"Total MST weight: " + std::to_string(graph->getTotalWeight_MST()) + "\n";

            });
            // Étape 2 : Analyse de la distance moyenne
            step2.enqueue([&]() {
                finalResult += std::string(15, ' ') + "Average distance: " + std::to_string(graph->getAverageDistance_MST()) + "\n";
            });
            // Étape 3 : Analyse des chemins
            step3.enqueue([&]() {
                finalResult += std::string(15, ' ') + "Longest path: " + graph->getTreeDepthPath_MST() + "\n";
                finalResult += std::string(15, ' ') + "Heaviest path: " + graph->getMaxWeightPath_MST() + "\n";
            });
            // Étape 4 : Analyse des arêtes
            step4.enqueue([&]() {
                finalResult += std::string(15, ' ') + "Heaviest edge: " + graph->getMaxWeightEdge_MST() + "\n";
                finalResult += std::string(15, ' ') + "Lightest edge: " + graph->getMinWeightEdge_MST() + "\n";
            });
            // Démarrage et arrêt des étapes
            step1.start(); step1.stop();
            step2.start(); step2.stop();
            step3.start(); step3.stop();
            step4.start(); step4.stop();

            // Envoi de la réponse finale au client
            send(client_socket, finalResult.c_str(), finalResult.size(), 0);

        }

        }

        // Close the client connection.
        close(client_socket);
    }
};

#endif // SERVER_PL_HPP