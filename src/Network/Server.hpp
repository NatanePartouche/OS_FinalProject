#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <memory>
#include <atomic>
#include <tuple>
#include "ActiveObject.hpp"

class Server {
public:
    Server(int port, int poolSize = 10);
    ~Server();

    void start();
    void stop();

private:
    int serverPort;
    int serverSocket;
    std::atomic<bool> running;
    std::map<int, std::shared_ptr<ActiveObject>> clients;
    std::vector<std::shared_ptr<ActiveObject>> aoPool;
    int nextAoIndex = 0;

    void handleClient(int clientSocket, std::shared_ptr<ActiveObject> ao);
    std::tuple<int, int, std::vector<std::tuple<int, int, int>>, std::string> requestGraphParametersFromClient(int newSocket);
};

#endif // SERVER_HPP