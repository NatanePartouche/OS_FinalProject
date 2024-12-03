#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <unordered_set>
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class Server {
protected:
    int port;                                      // Port d'écoute
    std::string address;                           // Adresse IP ou nom d'hôte
    std::unordered_set<int> connectedClients;      // Ensemble des clients connectés
    int server_fd;                                 // Descripteur de socket serveur
    std::mutex log_mutex;                          // Mutex pour les logs
    std::atomic<bool> running;                     // Indique si le serveur est actif

public:
    Server(const std::string& addr, int p)
        : port(p), address(addr), server_fd(-1), running(false) {
        if (port <= 0 || port > 65535) {
            throw std::invalid_argument("[Server] Invalid port. Must be between 1 and 65535.");
        }
        if (address.empty()) {
            throw std::invalid_argument("[Server] Invalid address. Cannot be empty.");
        }
    }

    ~Server() {stop();}

    // Méthodes abstraites pour l'extension
    virtual void start() = 0;
    virtual void stop() {
        if (!running.exchange(false)) {
            log("[Server] Server is already stopped.");
            return;
        }
        closeSocket();
        log("[Server] Server stopped.");
    }
    virtual void handleClient(int clientSocket) = 0;

    // Gestion des clients
    virtual bool addClient(int clientID) {
        if (connectedClients.find(clientID) != connectedClients.end()) {
            log("[Server] Client " + std::to_string(clientID) + " is already connected.");
            return false;
        }
        connectedClients.insert(clientID);
        log("[Server] Client " + std::to_string(clientID) + " connected.");
        return true;
    }
    virtual bool removeClient(int clientID) {
        if (connectedClients.erase(clientID)) {
            log("[Server] Client " + std::to_string(clientID) + " disconnected.");
            return true;
        }
        log("[Server] Client " + std::to_string(clientID) + " not found.");
        return false;
    }

protected:
    // Configurer le socket
    void setupServerSocket() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            throw std::runtime_error("[Server] Failed to create socket.");
        }

        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            closeSocket();
            throw std::runtime_error("[Server] Failed to set socket options.");
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            closeSocket();
            throw std::runtime_error("[Server] Bind failed.");
        }

        if (listen(server_fd, 10) < 0) {
            closeSocket();
            throw std::runtime_error("[Server] Listen failed.");
        }

        log("[Server] Socket configured and listening on " + address + ":" + std::to_string(port));
    }
    void closeSocket() {
        if (server_fd >= 0) {
            close(server_fd);
            log("[Server] Socket closed.");
            server_fd = -1;
        }
    }

    virtual void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);
        std::cout << message << std::endl;
    }
};

#endif // SERVER_HPP