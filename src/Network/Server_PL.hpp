#ifndef SERVER_PL_HPP
#define SERVER_PL_HPP

#include <thread>                   // For creating and managing threads.
#include <sstream>                  // For parsing client input commands.
#include "ActiveObject.hpp"         // ActiveObject for task execution.
#include "../../src/Model/Graph.hpp" // Graph model used for MST operations.

/**
 * @class Server_PL
 * @brief Implements a server using the Pipeline design pattern.
 */
class Server_PL : public Server {
public:
    /**
     * @brief Constructor to initialize the server.
     *
     * @param addr Server address (IP).
     * @param port Server port number.
     */
    Server_PL(const std::string& addr, int port) : Server(addr, port) {
        setupServerSocket(); // Sets up the server socket for communication.
        std::cout << "Server_PL configured on " << address << ":" << port << std::endl; // Inform about configuration.
    }

    /**
     * @brief Starts the Pipeline server.
     *
     * Continuously listens for incoming client connections and processes them using individual threads.
     */
    void start() override {
        if (running.exchange(true)) { // Prevents multiple server starts.
            std::cout << "Server_PL is already running." << std::endl;
            return;
        }

        std::cout << "Server_PL started." << std::endl;

        // Main loop to handle incoming client connections.
        while (running) {
            sockaddr_in client_addr{}; // Client address information.
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len); // Accept a client connection.

            if (client_socket < 0) { // Handle errors during connection.
                if (running) {
                    std::cerr << "Failed to accept connection." << std::endl;
                }
                continue;
            }

            std::cout << "New client connected: " << client_socket << std::endl;

            if (!addClient(client_socket)) { // Reject the connection if the client cannot be added.
                close(client_socket);
                continue;
            }

            // Handle the client connection in a separate thread.
            std::thread(&Server_PL::handleClient, this, client_socket).detach();

            // Vérifier s'il reste d'autres clients connectés.
            {
                std::lock_guard<std::mutex> lock(client_mutex); // Protection de la liste des clients.
                if (connectedClients.empty()) { // Si aucun client n'est connecté.
                    std::cout << "Stopping server..." << std::endl;
                    running = false; // Mettre `running` à false pour arrêter la boucle principale.
                }
            }

        }

        std::cout << "Server_PL has stopped accepting new connections." << std::endl;
    }

    void stop() override {

        if (!running.exchange(false)) { // Empêche les arrêts multiples.
            std::cout << "Server_LF is not running." << std::endl;
            return;
        }

        std::cout << "Stopping Server_LF..." << std::endl;

        // Fermer le socket du serveur pour interrompre `accept()`
        if (shutdown(server_fd, SHUT_RDWR) < 0) {
            std::cerr << "Error shutting down server socket: " << strerror(errno) << std::endl;
        }

        close(server_fd);
    }

    /**
     * @brief Handles client communication.
     *
     * Processes commands such as creating graphs, adding edges, and calculating MSTs.
     *
     * @param client_socket The socket descriptor for the client.
     */
    void handleClient(int client_socket) override {
        std::shared_ptr<Graph> graph; // Unique graph instance for this client.

        // Help menu to guide the client on available commands.
        std::string helpMenu = "------------------------ COMMAND MENU --------------------------------------------\n";
        helpMenu += "Create a new graph:\n   - Syntax: 'create <number_of_vertices>'\n";
        helpMenu += "Add an edge:\n   - Syntax: 'add <u> <v> <w>'\n";
        helpMenu += "Remove an edge:\n   - Syntax: 'remove <u> <v>'\n";
        helpMenu += "Choose MST Algorithm:\n   - Syntax: 'algo <algorithm_name>'\n     (prim/kruskal/tarjan/boruvka/integer_mst)\n";
        helpMenu += "Shutdown:\n   - Syntax: 'shutdown'\n";
        helpMenu += "----------------------------------------------------------------------------------\n";

        send(client_socket, helpMenu.c_str(), helpMenu.size(), 0); // Send help menu to the client.

        char buffer[1024]; // Buffer for client commands.

        while (running) { // Process client commands.
            int bytesRead = read(client_socket, buffer, sizeof(buffer));
            if (bytesRead <= 0) { // Handle client disconnection.
                std::cout << "Client disconnected." << std::endl;
                break;
            }

            std::string request(buffer, bytesRead); // Parse client command.
            std::stringstream ss(request);
            std::string command;
            ss >> command;

            // Handle specific commands.
            if (command == "create") { // Create a graph.
                std::string token;
                if (ss >> token) {
                    try {
                        int size = std::stoi(token);
                        if (size <= 0) { // Vérifie si le nombre de sommets est <= 0.
                            std::string response =
                                "Error: Number of vertices must be > 0.\n"
                                "Try again: create <number_of_vertices>\n";
                            send(client_socket, response.c_str(), response.size(), 0);
                        } else {
                            // Vérifie s'il y a des arguments supplémentaires
                            std::string extra;
                            if (ss >> extra) { // Arguments supplémentaires détectés.
                                std::string response =
                                    "Error: Too many arguments provided.\n"
                                    "Syntax: create <number_of_vertices>\n"
                                    "Example: create 5\n";
                                send(client_socket, response.c_str(), response.size(), 0);
                            } else { // Aucun argument supplémentaire, commande valide.
                                graph = std::make_unique<Graph>(size);
                                std::string response =
                                    "Graph created with " + std::to_string(size) + " vertices.\n";
                                send(client_socket, response.c_str(), response.size(), 0);
                            }
                        }
                    } catch (...) {
                        std::string response =
                            "Invalid input. Syntax: create <number_of_vertices>\n"
                            "Example: create 5\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    }
                } else {
                    std::string response =
                        "Error: Missing argument. Syntax: create <number_of_vertices>\n"
                        "Example: create 5\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
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
            else if (command == "algo") {
                if (!graph) { // Ensures a graph exists.
                    std::cerr << "Graph not initialized when trying to set algorithm." << std::endl;
                    std::string response = "Error: Graph not created. Use 'create' first.\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                    continue;
                }

                std::string selectedAlgorithm;
                if (ss >> selectedAlgorithm) { // Extracts the algorithm name.
                    if (selectedAlgorithm == "prim" || selectedAlgorithm == "kruskal" ||
                        selectedAlgorithm == "boruvka" || selectedAlgorithm == "tarjan" ||
                        selectedAlgorithm == "integer_mst") {
                        graph->_algorithmChoice = selectedAlgorithm; // Sets the algorithm.
                        std::string response = "Algorithm set to " + selectedAlgorithm + ".\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    } else { // Handles invalid algorithms.
                        std::string response = "Error: Unknown algorithm '" + selectedAlgorithm + "'.\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    }
                } else { // Handles missing algorithm name.
                    std::string response = "Invalid input. Syntax: 'algo <algorithm_name>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            else if (command == "shutdown") { // Command to disconnect the client from the server
                std::string response = "Shutting down client.\n";
                ssize_t bytes_sent = send(client_socket, response.c_str(), response.size(), 0);
                if (bytes_sent < 0) {
                    std::cerr << "Error sending response to client " << client_socket << ": " << strerror(errno) << std::endl;
                }

                // Optional: Wait a short moment to ensure the client receives the message
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                // Remove the client (also closes the socket and checks if the server should stop)
                if (removeClient(client_socket)) {
                    std::cout << "Client " << client_socket << " has been successfully removed and disconnected." << std::endl;
                } else {
                    std::cerr << "Failed to remove client " << client_socket << "." << std::endl;
                }

                // No need to lock the mutex again here if removeClient handles it

                break; // Exit handling for this client.
            }
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