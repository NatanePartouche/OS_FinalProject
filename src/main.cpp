#include "../src/Network/Server.hpp"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>

std::atomic<bool> serverRunning(true);

void signalHandler(int signal) {
    if (signal == SIGINT) {
        serverRunning = false;
        std::cout << "\nArrêt du serveur demandé..." << std::endl;
    }
}

int main() {
    // Associer SIGINT (Ctrl+C) à la fonction signalHandler
    std::signal(SIGINT, signalHandler);

    int port = 8080; // Port sur lequel le serveur va écouter
    int poolSize = 10; // Nombre d'ActiveObjects dans le pool, ajustable selon la charge

    // Créer un serveur avec un pool d'ActiveObjects
    Server server(port, poolSize);

    // Démarrer le serveur dans un thread séparé
    std::thread serverThread([&]() {
        server.start();
    });

    // Attendre le signal d'arrêt
    while (serverRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Arrêter le serveur proprement
    server.stop();

    // Rejoindre le thread du serveur
    if (serverThread.joinable()) {serverThread.join();}

    std::cout << "Serveur arrêté proprement." << std::endl;
    return 0;
}