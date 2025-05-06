#include "../headers/DSAServer.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.json>" << std::endl;
        std::cerr << "Example: " << argv[0] << " query.json" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    
    // Process the query from the input file
    DSAServer server;
    server.processQuery(inputFile);
    
    return 0;
}
