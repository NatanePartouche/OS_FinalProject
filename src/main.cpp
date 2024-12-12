#include <iostream>
#include <memory>
#include <string>
#include "../src/Network/Server.hpp"
#include "../src/Network/Server_LF.hpp"
#include "../src/Network/Server_PL.hpp"

int main(int argc, char* argv[]) {
    // Default configuration values (injected via CMake)
    std::string mode = DEFAULT_MODE; // Default server mode (e.g., -LF or -PL)
    int port = DEFAULT_PORT;         // Default port number
    int num_threads = 4;             // Default number of threads for multi-threaded servers

    // Process command-line arguments provided by the user
    if (argc >= 2) {
        mode = argv[1]; // Set mode to user input (e.g., -LF or -PL)
    }
    if (argc >= 3) {
        try {
            // Convert the second argument to an integer (number of threads)
            num_threads = std::stoi(argv[2]);
        } catch (...) {
            // Handle invalid input for the number of threads
            std::cerr << "Error: Invalid number of threads provided." << std::endl;
            return 1; // Exit with error code
        }
    }

    // Validate the input parameters
    if (num_threads <= 0) {
        // Ensure that the number of threads is a positive integer
        std::cerr << "Error: Number of threads must be greater than 0." << std::endl;
        return 1; // Exit with error code
    }
    if (port <= 0 || port > 65535) {
        // Ensure that the port number is within the valid range (1-65535)
        std::cerr << "Error: Port number must be between 1 and 65535." << std::endl;
        return 1; // Exit with error code
    }

    // Create a server instance dynamically based on the selected mode
    std::unique_ptr<Server> server;

    if (mode == "-LF") {
        // If the mode is Leader-Followers (-LF), create a server that uses multi-threading
        std::cout << "Starting Leader-Followers server on port " << port
                  << " with " << num_threads << " threads..." << std::endl;
        server = std::make_unique<Server_LF>("127.0.0.1", port, num_threads);
    }
    else if (mode == "-PL") {
        // If the mode is Pipeline (-PL), create a simpler server without threading
        std::cout << "Starting Pipeline server on port " << port << "..." << std::endl;
        server = std::make_unique<Server_PL>("127.0.0.1", port);
    }
    else {
        // Handle invalid mode input
        std::cerr << "Unknown mode: " << mode << std::endl;
        std::cerr << "Usage: " << argv[0] << " -PL|-LF [<num_threads>] [<port>]" << std::endl;
        return 1; // Exit with error code
    }

    // Start the server and allow it to run
    server->start();

    // Keep the program running until the user manually stops it
    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();

    // Gracefully stop the server and release resources
    server->stop();

    // Inform the user that the server has been shut down cleanly
    std::cout << "Server stopped gracefully." << std::endl;
    return 0; // Exit successfully
}