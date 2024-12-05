#include <thread>
#include <sstream>
#include <cerrno>    // Pour errno
#include <cstring>   // Pour strerror
#include "LeaderFollowers.hpp"       // Includes Leader-Followers thread pool implementation.
#include "../../src/Model/Graph.hpp" // Includes the Graph class for graph operations.

/**
 * @class Server_LF
 * @brief Implements a server using the Leader-Followers pattern.
 */
class Server_LF : public Server {

private:
    LeaderFollowers thread_pool; // Thread pool for handling client tasks.

public:

    /**
     * @brief Constructor for Leader-Followers server.
     *
     * @param addr Server address (IP).
     * @param port Server port number.
     * @param num_threads Number of threads in the pool.
     */
    Server_LF(const std::string& addr, int port, int num_threads)
        : Server(addr, port), thread_pool(num_threads) {
        setupServerSocket(); // Sets up the server socket.
        std::cout << "Server_LF configured on " << address << ":" << port << std::endl;
    }

    /**
     * @brief Starts the Leader-Followers server.
     *
     * Accepts client connections and assigns them to threads in the pool.
     */
    void start() override {
        if (running.exchange(true)) { // Empêche les démarrages multiples.
            std::cout << "Server_LF is already running." << std::endl;
            return;
        }

        std::cout << "Server_LF started." << std::endl;

        while (running) { // Boucle principale du serveur.
            sockaddr_in client_addr{}; // Informations sur le client.
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

            if (client_socket < 0) { // Gérer les erreurs lors de la connexion.
                if (!running) {
                    std::cout << "Server is shutting down. Exiting accept loop." << std::endl;
                    break; // Sortir de la boucle si le serveur est en cours d'arrêt.
                }
                std::cerr << "Failed to accept connection: " << strerror(errno) << std::endl;
                continue;
            }

            std::cout << "New client connected: " << client_socket << std::endl;

            if (!addClient(client_socket)) { // Rejeter la connexion si le client ne peut pas être ajouté.
                close(client_socket);
                continue;
            }

            // Assigner le client au pool de threads.
            thread_pool.add_task([this, client_socket]() {
                handleClient(client_socket);
            });
        }

        std::cout << "Server_LF has stopped accepting new connections." << std::endl;
    }

    void stop() override {

        thread_pool.stop();

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
     * Processes graph commands and responds to the client.
     *
     * @param client_socket The socket descriptor for the client.
     */
    void handleClient(int client_socket) override {
        std::shared_ptr<Graph> graph;

        // Prepare the help menu to send to the client.
        std::string helpMenu = "------------------------ COMMAND MENU --------------------------------------------\n";
        helpMenu += "Create a new graph:\n   - Syntax: 'create <number_of_vertices>'\n";
        helpMenu += "Add an edge:\n   - Syntax: 'add <u> <v> <w>'\n";
        helpMenu += "Remove an edge:\n   - Syntax: 'remove <u> <v>'\n";
        helpMenu += "Choose MST Algorithm:\n   - Syntax: 'algo <algorithm_name>'\n     (prim/kruskal/tarjan/boruvka/integer_mst)\n";
        helpMenu += "Shutdown:\n   - Syntax: 'shutdown'\n";
        helpMenu += "----------------------------------------------------------------------------------\n";

        send(client_socket, helpMenu.c_str(), helpMenu.size(), 0); // Send help menu to the client.

        char buffer[1024]; // Buffer to receive client commands.
        while (running) { // Process commands while the server is active.
            int bytesRead = read(client_socket, buffer, sizeof(buffer)); // Read data from the client.
            if (bytesRead <= 0) { // Handle client disconnection.
                std::cout << "Client disconnected." << std::endl;
                break;
            }

            // Parse the received command.
            std::string request(buffer, bytesRead);
            std::stringstream ss(request);
            std::string command;
            ss >> command;

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
            else if (command == "add") { // Add an edge.
                if (!graph) {
                    std::string response = "Graph not created. Use 'create' first.\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                    continue;
                }
                int u, v, weight;
                if (ss >> u >> v >> weight) {
                    graph->add_edge(u, v, weight);
                    std::string response = "Edge added: (" + std::to_string(u) + ", " + std::to_string(v) + ") with weight " + std::to_string(weight) + "\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                } else {
                    std::string response = "Invalid input. Syntax: 'add <u> <v> <w>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            else if (command == "remove") { // Remove an edge.
                if (!graph) {
                    std::string response = "Graph not created. Use 'create' first.\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                    continue;
                }
                int u, v;
                if (ss >> u >> v) {
                    graph->remove_edge(u, v);
                    std::string response = "Edge removed: (" + std::to_string(u) + ", " + std::to_string(v) + ")\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                } else {
                    std::string response = "Invalid input. Syntax: 'remove <u> <v>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            else if (command == "algo") { // Set MST algorithm.
                if (!graph) {
                    std::cerr << "Graph not initialized when trying to set algorithm." << std::endl;
                    std::string response = "Error: Graph not created. Use 'create' first.\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                    continue;
                }
                std::string selectedAlgorithm;
                if (ss >> selectedAlgorithm) {
                    if (selectedAlgorithm == "prim" || selectedAlgorithm == "kruskal" ||
                        selectedAlgorithm == "boruvka" || selectedAlgorithm == "tarjan" ||
                        selectedAlgorithm == "integer_mst") {
                        graph->_algorithmChoice = selectedAlgorithm;
                        std::string response = "Algorithm set to " + selectedAlgorithm + ".\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    } else {
                        std::string response = "Error: Unknown algorithm '" + selectedAlgorithm + "'.\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    }
                } else {
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
            else { // Handle unknown commands.
                std::string response = "Unknown command. Use 'help' for a list of commands.\n";
                send(client_socket, response.c_str(), response.size(), 0);
            }

            if (graph) {
                graph->Solve();
                std::string analysis = graph->Analysis(); // Assume this method summarizes MST analysis.
                send(client_socket, analysis.c_str(), analysis.size(), 0);
            }
        }

        close(client_socket); // Close the client connection.
        std::cout << "Client socket closed." << std::endl;
    }

};