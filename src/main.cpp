#include "Network/Server.hpp"
// Main function to start the server
int main() {
    Server server;      // Create an instance of the Server class
    server.runServer(); // Start the server using the created instance
    return 0;           // Return 0 to indicate successful execution
}