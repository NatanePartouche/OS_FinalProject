#include <thread>
#include <sstream>
#include "LeaderFollowers.hpp"       // Inclut la classe Leader-Followers pour gérer les threads.
#include "../../src/Model/Graph.hpp" // Inclut la classe Graph pour représenter les graphes.

/**
 * @brief Server_LF class - Implements a server based on the Leader-Followers pattern.
 *
 * This class extends the abstract `Server` class and uses a thread pool
 * to asynchronously manage client connections and requests.
 */
class Server_LF : public Server { // Hérite de la classe Server.
private:
    LeaderFollowers thread_pool; // Pool de threads basé sur le modèle Leader-Followers.

public:

    /**
     * @brief Constructor for Server_LF.
     *
     * Initializes the server with the provided address, port, and number of threads.
     * Configures the server socket and prepares it to handle incoming client connections.
     *
     * @param addr The IP address or hostname on which the server listens.
     * @param port The port on which the server listens for connections.
     * @param num_threads The number of threads in the Leader-Followers thread pool.
     */
    Server_LF(const std::string& addr, int port, int num_threads) : Server(addr, port), thread_pool(num_threads) {
        // Initialise le serveur et le pool de threads.
        setupServerSocket(); // Configure le socket du serveur.
        log("[Server_LF] Server configured on " + address + ":" + std::to_string(port)); // Journalise l'adresse et le port.
    }

    /**
     * @brief Starts the Leader-Followers server.
     *
     * This method enables the server, accepts incoming connections, and delegates their management
     * to a thread pool based on the Leader-Followers model.
     *
     * - If the server is already running, it will not start again.
     * - Each client connection is handled by adding tasks to the thread pool.
     *
     * @throws std::exception If a critical error occurs during execution.
     */
    void start() override {
        // Vérifie si le serveur est déjà en cours d'exécution.
        if (running.exchange(true)) { // `running` est un std::atomic<bool>.
            log("[Server_LF] Server is already running."); // Journalise l'état.
            return; // Évite les redémarrages multiples.
        }

        log("[Server_LF] Server started."); // Journalise que le serveur a démarré.

        // Boucle principale pour accepter les connexions tant que le serveur est actif.
        while (running) {
            sockaddr_in client_addr{}; // Structure pour stocker les informations sur le client.
            socklen_t client_len = sizeof(client_addr); // Taille de la structure `sockaddr_in`.
            // Accepte une connexion client.
            int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            // Si l'appel à `accept` échoue.
            if (client_socket < 0) {
                if (running) { // Vérifie que le serveur est toujours actif.
                    log("[Server_LF] Failed to accept connection."); // Journalise l'erreur.
                }
                continue; // Continue avec la prochaine itération.
            }
            log("[Server_LF] New client connected: " + std::to_string(client_socket)); // Journalise la connexion d'un nouveau client.
            // Ajoute le client à la liste des clients connectés.
            if (!addClient(client_socket)) { // Si l'ajout échoue.
                close(client_socket); // Ferme le socket pour éviter une fuite de ressources.
                continue;
            }
            // Ajoute une tâche au pool de threads pour gérer la connexion client.
            thread_pool.add_task([this, client_socket]() {
                handleClient(client_socket); // Gère les interactions avec le client.
            });
        }
    }

    /**
     * @brief Handles interactions with a client.
     *
     * Reads commands sent by the client, performs operations on the graph,
     * and sends responses back to the client.
     *
     * @param client_socket The client's socket descriptor.
     */
    void handleClient(int client_socket) override {
        std::shared_ptr<Graph> graph;

        // Prépare le menu d'aide à envoyer au client.
        std::string helpMenu = "------------------------ COMMAND MENU --------------------------------------------\n";
        helpMenu += "Create a new graph:\n   - Syntax: 'create <number_of_vertices>'\n";
        helpMenu += "Add an edge:\n   - Syntax: 'add <u> <v> <w>'\n";
        helpMenu += "Remove an edge:\n   - Syntax: 'remove <u> <v>'\n";
        helpMenu += "Choose MST Algorithm:\n   - Syntax: 'algo <algorithm_name>'\n     (prim/kruskal/tarjan/boruvka/integer_mst)\n" ;
        helpMenu += "Shutdown:\n   - Syntax: 'shutdown'\n";
        helpMenu += "----------------------------------------------------------------------------------\n";

        send(client_socket, helpMenu.c_str(), helpMenu.size(), 0); // Envoie le menu d'aide au client.

        char buffer[1024]; // Tampon pour recevoir les commandes du client.
        while (running) { // Tant que le serveur est actif.
            int bytesRead = read(client_socket, buffer, sizeof(buffer)); // Lit les données envoyées par le client.
            if (bytesRead <= 0) { // Si le client se déconnecte ou s'il y a une erreur.
                std::cout << "Client disconnected.\n";
                break;
            }

            // Traite la commande reçue.
            std::string request(buffer, bytesRead); // Convertit les données reçues en chaîne.
            std::stringstream ss(request); // Crée un flux à partir de la commande.
            std::string command;
            ss >> command; // Extrait la commande principale.

            if (command == "create") {
                std::string token;
                if (ss >> token) { // Vérifier si un argument est fourni après "create".
                    try {
                        int size = std::stoi(token); // Convertir l'argument en entier.
                        if (size < 0) {
                            std::string response = "Error: Number of vertices must be greater than or equal to 0.\n";
                            send(client_socket, response.c_str(), response.size(), 0);
                        } else {
                            graph = std::make_unique<Graph>(size); // Créer ou réinitialiser le graphe.
                            std::string response = "Graph created with " + std::to_string(size) + " vertices.\n";
                            send(client_socket, response.c_str(), response.size(), 0);
                        }
                    } catch (const std::invalid_argument&) {
                        // Si l'argument n'est pas un entier valide.
                        std::string response = "Invalid input. Syntax: 'create <number_of_vertices>'\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    } catch (const std::out_of_range&) {
                        // Si l'entier est trop grand ou trop petit.
                        std::string response = "Error: Number out of range.\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                    }
                } else {
                    // Aucun argument fourni : ne fait rien et indique l'erreur.
                    std::string response = "Error: Number of vertices not provided. Syntax: 'create <number_of_vertices>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            else if (command == "add") {
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
            else if (command == "remove") {
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
            else if (command == "algo") {
                if (!graph) {
                    log("[Server] Graph not initialized when trying to set algorithm.");
                    std::string response = "Error: Graph not created. Use 'create' first.\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                    continue; // Continue to the next iteration of the loop
                }

                log("[Server] Received 'algo' command.");
                std::string selectedAlgorithm;
                if (ss >> selectedAlgorithm) {
                    log("[Server] Algorithm selected: " + selectedAlgorithm);
                    if (selectedAlgorithm == "prim" || selectedAlgorithm == "kruskal" ||
                        selectedAlgorithm == "boruvka" || selectedAlgorithm == "tarjan" ||
                        selectedAlgorithm == "integer_mst") {
                        graph->_algorithmChoice = selectedAlgorithm;
                        log("[Server] Algorithm set successfully.");
                        std::string response = "Algorithm set to " + selectedAlgorithm + ".\n";
                        send(client_socket, response.c_str(), response.size(), 0);
                        } else {
                            log("[Server] Unknown algorithm: " + selectedAlgorithm);
                            std::string response = "Error: Unknown algorithm '" + selectedAlgorithm + "'.\n";
                            send(client_socket, response.c_str(), response.size(), 0);
                        }
                } else {
                    log("[Server] Invalid 'algo' command syntax.");
                    std::string response = "Invalid input. Syntax: 'algo <algorithm_name>'\n";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
            }
            else if (command == "shutdown") {
                std::string response = "Shutting down client.\n";
                send(client_socket, response.c_str(), response.size(), 0);
                break;
            }
            else {
                std::string response = "Unknown command. Use 'help' for a list of commands.\n";
                send(client_socket, response.c_str(), response.size(), 0);
            }
            if (graph) {
                graph->Solve();
                send(client_socket, graph->Analysis().c_str(), graph->Analysis().size(), 0);
            }
        }

        close(client_socket); // Ferme la connexion client.
        std::cout << "Client socket closed.\n"; // Indique que le socket est fermé.
    }

};