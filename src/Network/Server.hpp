#ifndef SERVER_HPP
#define SERVER_HPP

#include <atomic>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <set>
#include <tuple>
#include <vector>

#define PORT 8080            // The port number the server listens on
#define BUFFER_SIZE 1024     // Size of the buffer used to read data from clients
#define THREAD_POOL_SIZE 4   // Number of threads in the server's thread pool

class Server {
public:
    // Constructor and Destructor
    Server();
    ~Server();

    // Main server function using the Leader-Follower pattern
    void runServer();

    // Function to close all active client connections
    void closeAllClients();

    // Function to handle a single client connection
    void handleClient(int clientSocket);

    // Function to request graph parameters from client
    std::tuple<int, int, std::vector<std::tuple<int, int, int>>, std::string> requestGraphParametersFromClient(int newSocket);

private:
    // Thread synchronization variables
    std::mutex leaderMutex;                         // Mutex for the Leader-Follower pattern
    std::condition_variable leaderCV;               // Condition variable for managing leader threads
    std::queue<int> clientQueue;                    // Queue to hold the client sockets waiting to be processed
    std::set<int> activeClientSockets;              // Set to store all active client sockets
    std::mutex clientSocketMutex;                   // Mutex to protect access to activeClientSockets

    // Server state variables
    std::atomic<bool> serverRunning{true};          // Atomic flag to indicate if the server is running
    std::atomic<int> activeClients{0};              // Counter for the number of currently active clients
    int serverFd;                                   // File descriptor for the server socket
};

#endif // SERVER_HPP