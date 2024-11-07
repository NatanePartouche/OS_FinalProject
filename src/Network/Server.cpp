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

// Fonction principale du serveur utilisant le modèle Leader-Follower
void Server::runServer() {
    struct sockaddr_in address;       // Structure pour l'adresse du socket
    int opt = 1;                      // Option pour configurer les options du socket
    int addrlen = sizeof(address);    // Taille de l'adresse
    int newSocket;                    // Socket pour les nouvelles connexions client

    // Création d'un ActiveObject avec un pool de threads de taille THREAD_POOL_SIZE
    ActiveObject activeObject(THREAD_POOL_SIZE);

    // Bloc de configuration et de liaison du serveur
    {
    // Création du socket serveur
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed"); // Affiche une erreur si la création du socket échoue
        exit(EXIT_FAILURE);
    }

    // Configuration des options du socket pour permettre la réutilisation de l'adresse et du port
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt"); // Affiche une erreur si la configuration des options échoue
        exit(EXIT_FAILURE);
    }

    // Spécifie la famille d'adresses (IPv4) et configure le port pour le socket
    address.sin_family = AF_INET; // Utilise l'adressage IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Accepte les connexions depuis n'importe quelle adresse IP
    address.sin_port = htons(PORT); // Convertit le port en ordre d'octets réseau et le configure

    // Lie le socket à l'adresse et au port spécifiés
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Bind failed"); // Affiche une erreur si la liaison échoue
        exit(EXIT_FAILURE);
    }

    // Met le socket en mode écoute pour accepter les connexions entrantes
    if (listen(serverFd, 10) < 0) {
        perror("Listen failed"); // Affiche une erreur si l'écoute échoue
        exit(EXIT_FAILURE);
    }
    std::cout << "Server is running and listening on port " << PORT << std::endl; // Indique que le serveur est prêt
    }

    // Création d'un pool de threads pour gérer les demandes des clients (vecteur de 4 threads)
    std::vector<std::thread> threadPool;
    for (int i = 0; i < THREAD_POOL_SIZE; ++i){
        // Chaque thread du pool exécute cette fonction lambda
        threadPool.emplace_back([&](){
            // Tant que le serveur fonctionne
            while (serverRunning) {
                int clientSocket;

                // Mécanisme Leader-Follower : un thread agit comme leader et traite les nouvelles connexions
                {
                    std::unique_lock<std::mutex> lock(leaderMutex); // Verrouille le mutex pour un accès thread-safe
                    leaderCV.wait(lock, [&]() { return !clientQueue.empty() || !serverRunning; }); // Attend de nouvelles connexions ou l'arrêt du serveur

                    // Si le serveur est en arrêt, quitte la boucle et termine le thread
                    if (!serverRunning) {return;}

                    // Si des connexions clients sont en attente
                    if (!clientQueue.empty()) {
                        clientSocket = clientQueue.front();  // Récupère le prochain socket client dans la file
                        clientQueue.pop();                   // Supprime le client de la file
                    }
                }

                // Traite les requêtes client de manière asynchrone avec le modèle ActiveObject
                activeObject.enqueueTask([this, clientSocket]() {
                    handleClient(clientSocket); // Traite le client dans une tâche séparée
                });
            }
        });
    }

    // Thread pour écouter les commandes de fermeture depuis la console du serveur
    std::thread shutdownThread([&](){
        std::string input;
        // Tant que le serveur fonctionne
        while (serverRunning) {
            std::cin >> input; // Attend une entrée utilisateur dans la console

            // Si l'entrée est "shutdown"
            if (input == "shutdown") {
                std::cout << "Server shutting down...\n"; // Affiche un message d'arrêt
                serverRunning = false;  // Met à jour l'état du serveur pour indiquer l'arrêt

                // Arrête le socket du serveur pour empêcher de nouvelles connexions
                shutdown(serverFd, SHUT_RDWR); // Désactive les opérations de lecture/écriture sur le socket du serveur
                close(serverFd);  // Ferme le socket du serveur
                closeAllClients(); // Ferme toutes les connexions clients actives
                leaderCV.notify_all();  // Notifie tous les threads en attente sur la variable conditionnelle pour qu'ils s'arrêtent
                break; // Quitte le thread de fermeture
            }
        }
    });

    // Boucle principale pour accepter les connexions clients
    while (serverRunning){
        // Accepte une nouvelle connexion client
        newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        // Si un nouveau client est connecté
        if (newSocket >= 0) {
            std::cout << "New client connection accepted.\n";
            {
                std::lock_guard<std::mutex> lock(leaderMutex); // Verrouille le mutex pour un accès sécurisé à la file
                clientQueue.push(newSocket); // Ajoute le socket client à la file
            }
            leaderCV.notify_one(); // Notifie un thread pour gérer ce client
        }

        // Si le serveur est en cours d'arrêt, quitte la boucle
        else if (!serverRunning){break; }
    }

    // Attend la fin du thread de fermeture
    shutdownThread.join();

    // Attend la fin de tous les threads du pool pour s'assurer qu'ils terminent proprement
    for (auto &th : threadPool){
        // Si le thread peut être rejoint, attend sa fin
        if (th.joinable()) {th.join();}
    }

    std::cout << "Server has shut down immediately.\n"; // Affiche un message confirmant l'arrêt du serveur
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

// Function to handle a single client connection
void Server::handleClient(int clientSocket) {

    char buffer[BUFFER_SIZE] = {0}; // Buffer to store incoming client data

    ////////////////////////////////////////////////////////////////////////////////
    std::unique_ptr<Graph> graph;         // Pointer to store the client's graph
    std::unique_ptr<Graph> mst;           // Pointer to store the client's computed MST
    std::string algorithmChoice = "prim"; // Store the chosen algorithm for MST computation
    MSTFactory* algo = nullptr;           // Pointer to MST algorithm factory
    ////////////////////////////////////////////////////////////////////////////////

    // Add the client socket to the set of active clients
    {
        std::lock_guard<std::mutex> lock(clientSocketMutex);
        activeClientSockets.insert(clientSocket);
    }
    activeClients++; // Increment the counter of active clients

    // Display the help menu with available commands
    std::string helpMenu = "------------------------ COMMAND MENU --------------------------------------------\n";
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
    helpMenu += "   - Shows the Graph, MST of the Graph and an analysis summary.\n";
    helpMenu += "Shutdown:\n";
    helpMenu += "   - Syntax: 'shutdown'\n";
    helpMenu += "   - Closes the connection with the server.\n";
    helpMenu += "----------------------------------------------------------------------------------\n";
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
                }
                // Send an error response if the algorithm choice is invalid
                else { std::string response = "Error: Unknown algorithm '" + selectedAlgorithm + "'. Available options: prim, kruskal, boruvka, tarjan, integer_mst.\n";
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

                // Compute MST
                if (algo) { mst = std::make_unique<Graph>(algo->solveMST(*graph)); }
                else {
                    std::string response = "Unknown algorithm.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                    return;
                }

                std::string response = graph->displayGraph();
                response += mst->displayMST();

                // MST analysis metrics
                int totalWeight = mst->getTotalWeight();
                std::string longestPath = mst->getTreeDepthPath_MST();
                std::string heaviestEdge = mst->getMaxWeightEdge_MST();
                std::string heaviestPath = mst->getMaxWeightPath_MST();
                std::string lightestEdge = mst->getMinWeightEdge_MST();
                double averageDistance = mst->getAverageDistance_MST();

                response += "------------------MST Analysis-------------------------\n";
                response += "Algorithm: " + algorithmChoice + "\n";
                response += "Total MST weight: " + std::to_string(totalWeight) + "\n";
                response += "Longest path: " + longestPath + "\n";
                response += "Heaviest path: " + heaviestPath + "\n";
                response += "Average distance: " + std::to_string(averageDistance) + "\n";
                response += "Heaviest edge: " + heaviestEdge + "\n";
                response += "Lightest edge: " + lightestEdge + "\n";
                response += "-------------------------------------------------------\n";

                send(clientSocket, response.c_str(), response.size(), 0);
            });
        }
        // Handle the "shutdown" command to terminate client connection
        else if (command == "shutdown") {
            pipeline.addStep([&]() {
                std::string response = "Shutting down client.\n";
                send(clientSocket, response.c_str(), response.size(), 0);
                std::cout << "Client initiated shutdown.\n";

                // Close client socket
                close(clientSocket);
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