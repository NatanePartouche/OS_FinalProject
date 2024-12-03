#include <iostream>
#include <memory>
#include <string>
#include "../src/Network/Server.hpp"
#include "../src/Network/Server_LF.hpp"
#include "../src/Network/Server_PL.hpp"

int main(int argc, char* argv[]) {
    // Vérifiez les arguments fournis par l'utilisateur
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " -PL|-LF [<num_threads>] [<port>]" << std::endl;
        return 1;
    }

    std::string mode = argv[1];   // Mode choisi : -PL ou -LF
    int num_threads = 4;         // Nombre de threads par défaut
    int port = 8080;             // Port par défaut

    // Lire le nombre de threads et le port, si fournis
    if (argc >= 3) {
        try {
            num_threads = std::stoi(argv[2]);
        } catch (...) {
            std::cerr << "Error: Invalid number of threads." << std::endl;
            return 1;
        }
    }
    if (argc >= 4) {
        try {
            port = std::stoi(argv[3]);
        } catch (...) {
            std::cerr << "Error: Invalid port number." << std::endl;
            return 1;
        }
    }

    // Vérifiez que les paramètres sont valides
    if (num_threads <= 0) {
        std::cerr << "Error: Number of threads must be greater than 0." << std::endl;
        return 1;
    }
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Port must be between 1 and 65535." << std::endl;
        return 1;
    }

    // Créez un pointeur vers le serveur
    std::unique_ptr<Server> server;

    try {
        // Instanciez le serveur en fonction du mode choisi
        if (mode == "-LF") {
            std::cout << "Starting Leader-Followers server on port " << port
                      << " with " << num_threads << " threads..." << std::endl;
            server = std::make_unique<Server_LF>("127.0.0.1", port, num_threads);
        } else if (mode == "-PL") {
            std::cout << "Starting Pipeline server on port " << port << "..." << std::endl;
            server = std::make_unique<Server_PL>("127.0.0.1", port); // Supprime num_threads
        } else {
            std::cerr << "Unknown mode: " << mode << std::endl;
            return 1;
        }

        // Démarrer le serveur
        server->start();

        // Rester actif jusqu'à ce que l'utilisateur appuie sur Entrée
        std::cout << "Press Enter to stop the server..." << std::endl;
        std::cin.get();

        // Arrêter le serveur proprement
        server->stop();

    } catch (const std::exception& e) {
        // Capture les erreurs et affiche un message approprié
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server stopped gracefully." << std::endl;
    return 0;
}