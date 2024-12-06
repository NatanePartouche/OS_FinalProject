#include <iostream>
#include <memory>
#include <string>
#include "../src/Network/Server.hpp"
#include "../src/Network/Server_LF.hpp"
#include "../src/Network/Server_PL.hpp"

int main(int argc, char* argv[]) {
    // Valeurs par défaut injectées par CMake
    std::string mode = DEFAULT_MODE; // Défini dans CMake
    int port = DEFAULT_PORT;         // Défini dans CMake
    int num_threads = 4;             // Par défaut : 4 threads

    // Lecture des arguments fournis par l'utilisateur
    if (argc >= 2) {
        mode = argv[1]; // Mode utilisateur : -LF ou -PL
    }
    if (argc >= 3) {
        try {
            num_threads = std::stoi(argv[2]);
        } catch (...) {
            std::cerr << "Error: Invalid number of threads provided." << std::endl;
            return 1;
        }
    }

    // Vérification des paramètres
    if (num_threads <= 0) {
        std::cerr << "Error: Number of threads must be greater than 0." << std::endl;
        return 1;
    }
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Port number must be between 1 and 65535." << std::endl;
        return 1;
    }

    // Créer un serveur basé sur le mode choisi
    std::unique_ptr<Server> server;
    if (mode == "-LF") {
        std::cout << "Starting Leader-Followers server on port " << port
                  << " with " << num_threads << " threads..." << std::endl;
        server = std::make_unique<Server_LF>("127.0.0.1", port, num_threads);
    }
    else if (mode == "-PL") {
        std::cout << "Starting Pipeline server on port " << port << "..." << std::endl;
        server = std::make_unique<Server_PL>("127.0.0.1", port); // Supprime num_threads
    }
    else {
        std::cerr << "Unknown mode: " << mode << std::endl;
        std::cerr << "Usage: " << argv[0] << " -PL|-LF [<num_threads>] [<port>]" << std::endl;
        return 1;
    }

    // Démarrer le serveur
    server->start();

    // Rester actif jusqu'à ce que l'utilisateur appuie sur Entrée
    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();

    // Arrêter le serveur proprement
    server->stop();

    std::cout << "Server stopped gracefully." << std::endl;
    return 0;
}