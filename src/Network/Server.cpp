#include "Server.hpp"
#include "../../src/Model/MST.hpp"
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

Server::Server(int port, int poolSize) : serverPort(port), serverSocket(-1), running(true) {
    for (int i = 0; i < poolSize; ++i) {
        aoPool.push_back(std::make_shared<ActiveObject>());
    }
}

Server::~Server() {stop();}

void Server::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Erreur lors de la création du socket serveur");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddress = {};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Erreur lors de la liaison du socket");
        exit(EXIT_FAILURE);
    }
    if (listen(serverSocket, 5) < 0) {
        perror("Erreur lors de l'écoute du socket");
        exit(EXIT_FAILURE);
    }

    std::cout << "Serveur démarré et en écoute sur le port " << serverPort << std::endl;

    while (running) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket >= 0) {
            auto& ao = aoPool[nextAoIndex % aoPool.size()];
            nextAoIndex++;
            clients[clientSocket] = ao;

            ao->enqueueTask([this, clientSocket, ao]() {
                this->handleClient(clientSocket, ao);
            });
        }
    }
}

void Server::stop() {
    running = false;
    if (serverSocket >= 0) {
        close(serverSocket);
    }
    for (auto& [socket, ao] : clients) {
        close(socket);
        ao->stop();
    }
    clients.clear();
}

void Server::handleClient(int clientSocket, std::shared_ptr<ActiveObject> ao) {

    auto graph = std::make_shared<Graph>(0);
    auto mst = std::make_shared<MST>(*graph, "Kruskal");

    // Initialisation du graphe avec les paramètres du client
    auto graphParameters = requestGraphParametersFromClient(clientSocket);
    int numVertices = std::get<0>(graphParameters);
    std::vector<std::tuple<int, int, int>> edges = std::get<2>(graphParameters);
    std::string algorithmChoice = std::get<3>(graphParameters);

    // Tâche asynchrone d'initialisation du graphe
    ao->enqueueTask([clientSocket, numVertices, edges, algorithmChoice, graph, mst]() {
        *graph = Graph(numVertices);
        for (const auto& edge : edges) {
            graph->addEdge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
        }
        *mst = MST(*graph, algorithmChoice);
        std::string response = "Graph and MST initialized successfully.\n";
        send(clientSocket, response.c_str(), response.size(), 0);
    });

    // Boucle pour gérer les commandes du client
    int choice = 0;
    char buffer[1024] = {0};
    while (choice != 9) {

        // Envoi du menu au client
        std::string menu = "\nPlease select an operation from the list below:\n"
                           "1. Calculate the total weight of the MST\n"
                           "2. Find the longest path between any two vertices in the MST\n"
                           "3. Calculate the average path length between all vertex pairs in the MST\n"
                           "4. Find the shortest path between two specified vertices\n"
                           "5. Add a new edge to the graph\n"
                           "6. Remove an existing edge from the graph\n"
                           "7. Create a new graph\n"
                           "8. Display the current graph\n"
                           "9. Exit\n"
                           "Enter the number corresponding to your choice : ";
        send(clientSocket, menu.c_str(), menu.size(), 0);

        int valread = read(clientSocket, buffer, 1024);
        if (valread <= 0) {
            std::cout << "Client déconnecté.\n";
            return;
        }
        choice = std::stoi(std::string(buffer));

        switch (choice) {
            case 1: {  // Calculer le poids total du MST
                ao->enqueueTask([clientSocket, mst]() {
                    std::string response = "\nThe total weight of the MST is: " + std::to_string(mst->getTotalWeight()) + "\n\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                });
                break;
            }
            case 2: {  // Trouver le chemin le plus long dans le MST
                ao->enqueueTask([clientSocket, mst]() {
                    std::string response = "\nLongest path calculated.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                });
                break;
            }
            case 3: {  // Calculer la distance moyenne dans le MST
                ao->enqueueTask([clientSocket, mst]() {
                    std::string response = "\nAverage path length calculated.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                });
                break;
            }
            case 4: {  // Trouver le chemin le plus court entre deux sommets
                ao->enqueueTask([clientSocket, mst]() {
                    std::string response = "Shortest path found.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                });
                break;
            }
            case 5: {  // Ajouter une nouvelle arête
                ao->enqueueTask([clientSocket, graph]() {
                    send(clientSocket, "Enter edge to add in format <u> <v> <weight>:\n", 45, 0);
                    char buffer[1024];
                    int valread = read(clientSocket, buffer, 1024);
                    if (valread > 0) {
                        std::istringstream iss(buffer);
                        int u, v, weight;
                        iss >> u >> v >> weight;
                        graph->addEdge(u, v, weight);
                        std::string response = "Edge added successfully.\n";
                        send(clientSocket, response.c_str(), response.size(), 0);
                    }
                });
                break;
            }
            case 6: {  // Supprimer une arête existante
                ao->enqueueTask([clientSocket, graph]() {
                    send(clientSocket, "Enter edge to remove in format <u> <v>:\n", 39, 0);
                    char buffer[1024];
                    int valread = read(clientSocket, buffer, 1024);
                    if (valread > 0) {
                        std::istringstream iss(buffer);
                        int u, v;
                        iss >> u >> v;
                        graph->removeEdge(u, v);
                        std::string response = "Edge removed successfully.\n";
                        send(clientSocket, response.c_str(), response.size(), 0);
                    }
                });
                break;
            }
            case 7: {  // Créer un nouveau graphe
                auto newGraphParameters = this->requestGraphParametersFromClient(clientSocket);
                int newNumVertices = std::get<0>(newGraphParameters);
                std::vector<std::tuple<int, int, int>> newEdges = std::get<2>(newGraphParameters);
                std::string newAlgorithmChoice = std::get<3>(newGraphParameters);

                ao->enqueueTask([this, clientSocket, graph, mst, newNumVertices, newEdges, newAlgorithmChoice]() {
                    *graph = Graph(newNumVertices);
                    for (const auto& edge : newEdges) {
                        graph->addEdge(std::get<0>(edge), std::get<1>(edge), std::get<2>(edge));
                    }
                    *mst = MST(*graph, newAlgorithmChoice);

                    std::string response = "New graph and MST created successfully.\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                });
                break;
            }
            case 8: {  // Afficher le graphe actuel
                ao->enqueueTask([clientSocket, mst]() {
                    std::string response = "\n" + mst->displayGraph() + "\n";
                    send(clientSocket, response.c_str(), response.size(), 0);
                });
                break;
            }
            case 9: {  // Quitter le menu
                ao->enqueueTask([clientSocket]() {
                    send(clientSocket, "Exiting the menu.\n", 18, 0);
                });
                break;
            }
            default: {
                ao->enqueueTask([clientSocket]() {
                    send(clientSocket, "Invalid choice. Please enter a number between 1 and 9.\n", 55, 0);
                });
                break;
            }
        }
    }
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
