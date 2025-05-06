#include "../headers/DSAServer.h"
#include "../headers/DecisionTree.h"
#include "../headers/Graph.h"
#include "../headers/HashTable.h"
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
// #include <thread>
// #include <future>
// #include <chrono>
// #include <mutex>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <filesystem>

using json = nlohmann::json;

// namespace ServerConfig {
//     const int CACHE_EXPIRY_SECONDS = 300;
//     const int MAX_CACHE_SIZE = 1000;
// }

void DSAServer::processQuery(const std::string& inputFile) {
    std::cout << "Processing query from: " << inputFile << std::endl;
    
    // Get the directory of the executable
    std::string exeDir = std::filesystem::current_path().string();
    
    // Initialize data structures
    DecisionTree decisionTree;
    Graph graph;
    HashTable hashTable;
    
    try {
        decisionTree.initialize();
        graph.initialize();
        hashTable.initializeDSAKnowledge();
        hashTable.loadFromFile(exeDir + "/responses.json"); // Load responses from file
        std::cout << "Successfully initialized all data structures" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize data structures: " << e.what() << std::endl;
        
        json errorResponse;
        errorResponse["error"] = std::string("Failed to initialize data structures: ") + e.what();
        std::cout << errorResponse.dump() << std::endl;
        return;
    }
    
    try {
        // Read input JSON file
        std::ifstream input(inputFile);
        if (!input.is_open()) {
            throw std::runtime_error("Failed to open input file: " + inputFile);
        }
        
        json inputJson;
        input >> inputJson;
        
        std::string userQuery = inputJson["query"].get<std::string>();
        
        // Special case for health check
        if (userQuery == "__health_check__") {
            json healthResponse;
            healthResponse["status"] = "healthy";
            healthResponse["decision_tree_status"] = decisionTree.isHealthy();
            healthResponse["graph_status"] = graph.isHealthy();
            healthResponse["hash_table_status"] = hashTable.isHealthy();
            std::cout << healthResponse.dump() << std::endl;
            return;
        }
        
        if (userQuery.empty()) {
            throw std::runtime_error("Query cannot be empty");
        }
        
        // Process query using all available algorithms
        auto decisionTreeResponse = decisionTree.traverse();
        auto graphAnalysis = graph.analyze(userQuery);
        auto hashTableResult = hashTable.lookup(userQuery); // Use lookup instead of get
        
        // Combine results
        json response;
        response["decision_tree_response"] = decisionTreeResponse;
        response["graph_analysis"] = graphAnalysis;
        response["hash_table_result"] = hashTableResult;
        response["query_received"] = userQuery;
        
        std::cout << response.dump() << std::endl;
        
    } catch (const std::exception& e) {
        json errorResponse;
        errorResponse["error"] = std::string("Error processing query: ") + e.what();
        std::cout << errorResponse.dump() << std::endl;
    }
}

// Implementation of the startServer function
void startServer() {
    try {
        std::cerr << "This function is no longer used. The program now operates in command-line mode." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
