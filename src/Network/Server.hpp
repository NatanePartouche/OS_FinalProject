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
#include <cerrno>    // Pour errno
#include <cstring>   // Pour strerror

/**
 * @class Server
 * @brief Abstract base class for a server implementation.
 *
 * Provides common functionality for managing clients, sockets, and server lifecycle.
 * Derived classes must implement specific behaviors like handling client connections and starting the server.
 */
class Server {
protected:
    int port;                                      ///< Listening port for the server.
    std::string address;                           ///< IP address or hostname.
    std::unordered_set<int> connectedClients;      ///< Set of currently connected clients.
    int server_fd;                                 ///< Server socket file descriptor.
    std::mutex client_mutex;                       ///< Mutex to ensure thread-safe client management.
    std::atomic<bool> running;                     ///< Indicates whether the server is running.

public:

    /**
     * @brief Constructor for the Server class.
     *
     * Initializes the server with the specified address and port. Ensures that the parameters are valid.
     *
     * @param addr The IP address or hostname for the server.
     * @param p The port number for the server.
     * @throws std::invalid_argument If the port is not within the range [1, 65535] or the address is empty.
     */
    Server(const std::string& addr, int p)
        : port(p), address(addr), server_fd(-1), running(false) {
        if (port <= 0 || port > 65535) {
            throw std::invalid_argument("Invalid port. Must be between 1 and 65535.");
        }
        if (address.empty()) {
            throw std::invalid_argument("Invalid address. Cannot be empty.");
        }
    }

    /**
     * @brief Destructor for the Server class.
     *
     * Ensures the server is stopped and resources are released.
     */
    ~Server(){}

    /**
     * @brief Starts the server.
     *
     * Must be implemented by derived classes.
     */
    virtual void start() = 0;

    /**
     * @brief Stops the server.
     *
     * Closes the server socket and releases resources.
     */
    virtual void stop() = 0;

    /**
     * @brief Handles a connected client.
     *
     * Must be implemented by derived classes to define specific client interaction logic.
     *
     * @param clientSocket The file descriptor of the client's socket.
     */
    virtual void handleClient(int clientSocket) = 0;

    /**
     * @brief Adds a client to the server's list of connected clients.
     *
     * @param clientID The file descriptor of the client's socket.
     * @return `true` if the client was successfully added; `false` if the client is already connected.
     */
    virtual bool addClient(int clientID) {
        std::lock_guard<std::mutex> lock(client_mutex); // Ensure thread-safe access.
        if (connectedClients.find(clientID) != connectedClients.end()) {
            std::cout << "Client " << clientID << " is already connected." << std::endl;
            return false;
        }
        connectedClients.insert(clientID); // Add the client to the set.
        std::cout << "Client " << clientID << " connected successfully." << std::endl;
        return true;
    }

    /**
     * @brief Removes a client from the server's list of connected clients.
     *
     * @param clientID The file descriptor of the client's socket.
     * @return `true` if the client was successfully removed; `false` if the client was not found.
     */
    virtual bool removeClient(int clientID) {
        std::lock_guard<std::mutex> lock(client_mutex); // Assurer un accès thread-safe.

        if (connectedClients.erase(clientID)) {
            // Fermer le socket du client (si ce n'est pas déjà fait)
            if (shutdown(clientID, SHUT_RDWR) < 0) {
                std::cerr << "Erreur lors du shutdown du socket du client " << clientID << ": " << strerror(errno) << std::endl;
            }
            if (close(clientID) < 0) {
                std::cerr << "Erreur lors de la fermeture du socket du client " << clientID << ": " << strerror(errno) << std::endl;
            } else {
                std::cout << "Socket du client " << clientID << " fermé avec succès." << std::endl;
            }

            std::cout << "Client " << clientID << " déconnecté." << std::endl;

            // Vérifier si le serveur doit s'arrêter
            if (connectedClients.empty()) {
                std::cout << "No clients are connected." << std::endl;
                stop();
            }

            return true;
        }

        std::cout << "Client " << clientID << " non trouvé." << std::endl;
        return false;
    }

protected:
    /**
     * @brief Configures the server socket.
     *
     * Sets up the socket for accepting client connections. Binds the socket to the address and port, and starts listening.
     *
     * @throws std::runtime_error If any step in socket setup fails.
     */
    void setupServerSocket() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket.
        if (server_fd == -1) {
            throw std::runtime_error("Failed to create socket.");
        }

        int opt = 1;
        // Set socket options to allow reusing the address.
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            closeSocket();
            throw std::runtime_error("Failed to set socket options.");
        }

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET; // Use IPv4.
        server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections on any interface.
        server_addr.sin_port = htons(port); // Set the port.

        // Bind the socket to the specified address and port.
        if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            closeSocket();
            throw std::runtime_error("Failed to bind the socket.");
        }

        // Start listening for incoming connections.
        if (listen(server_fd, 10) < 0) {
            closeSocket();
            throw std::runtime_error("Failed to start listening.");
        }

        std::cout << "Server socket configured and listening on " << address << ":" << port << std::endl;
    }

    /**
     * @brief Closes the server socket.
     *
     * Ensures the socket is properly closed to free resources.
     */
    void closeSocket() {
        if (server_fd >= 0) { // Check if the socket is valid.
            close(server_fd); // Close the socket.
            std::cout << "Server socket closed." << std::endl;
            server_fd = -1;
        }
    }

};

#endif // SERVER_HPP