#include "../headers/DSAKnowledge.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

DSAKnowledge::DSAKnowledge(HashTable& ht) 
    : hashTable(ht), 
      initialized(false),
      codeKeywords{"implement", "code", "example", "how to", "write"},
      complexityKeywords{"time complexity", "big o", "complexity", "efficiency", "space complexity"} {
    initialize();
}

DSAKnowledge::~DSAKnowledge() {
    // No cleanup needed; HashTable manages lifecycle
}

void DSAKnowledge::initialize() {
    std::cout << "Initializing DSA knowledge base..." << std::endl;
    
    // Load data from JSON file
    loadFromFile("dsa_knowledge.json");
    
    // Populate default data if JSON is missing or empty
    if (dsaExplanations.empty()) {
        dsaExplanations["linked list"] = "A linked list is a linear data structure where elements are stored in nodes, each containing data and a reference to the next node.";
        dsaExplanations["array"] = "An array is a collection of elements stored in contiguous memory, offering fast access via indices.";
    }
    
    if (timeComplexities.empty()) {
        timeComplexities["linked list"] = {
            {"insert_front", "O(1)"},
            {"insert_end", "O(1) with tail pointer, O(n) without"},
            {"delete", "O(1) if node given, O(n) to find"},
            {"search", "O(n)"}
        };
        timeComplexities["array"] = {
            {"access", "O(1)"},
            {"insert", "O(n)"},
            {"delete", "O(n)"},
            {"search", "O(n), O(log n) if sorted"}
        };
    }
    
    if (spaceComplexities.empty()) {
        spaceComplexities["linked list"] = "O(n)";
        spaceComplexities["array"] = "O(n)";
    }
    
    if (codeExamples.empty()) {
        codeExamples["linked list"] = R"(
struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};)";
        codeExamples["array"] = R"(
int arr[5] = {1, 2, 3, 4, 5}; // Simple array declaration
)";
    }
    
    initialized = true;
    std::cout << "DSA knowledge base initialized successfully!" << std::endl;
}

std::string DSAKnowledge::generateResponse(const std::string& query) const {
    if (!initialized) {
        return "I'm sorry, my knowledge base has not been initialized properly.";
    }

    std::string lowerQuery = hashTable.toLower(query); // Reuse HashTable's utility
    std::vector<std::string> keywords = hashTable.extractKeywordsFromQuery(lowerQuery, HashTable::STOP_WORDS);

    // Identify primary DSA topic
    std::string topic;
    for (const auto& keyword : keywords) {
        if (dsaExplanations.find(keyword) != dsaExplanations.end()) {
            topic = keyword;
            break;
        }
    }

    if (topic.empty()) {
        return "I couldn't identify a specific DSA topic. Please rephrase or ask about a data structure or algorithm.";
    }

    // Determine query intent
    bool wantsCode = false, wantsComplexity = false, wantsSpace = false;
    for (const auto& kw : codeKeywords) {
        if (lowerQuery.find(kw) != std::string::npos) {
            wantsCode = true;
            break;
        }
    }
    for (const auto& kw : complexityKeywords) {
        if (lowerQuery.find(kw) != std::string::npos) {
            wantsComplexity = true;
            wantsSpace = (lowerQuery.find("space") != std::string::npos);
            break;
        }
    }

    // Build response
    std::stringstream response;
    if (!wantsCode && !wantsComplexity) {
        response << dsaExplanations.at(topic) << "\n\n";
    }
    if (wantsCode && codeExamples.count(topic)) {
        response << "Implementation example:\n" << codeExamples.at(topic) << "\n\n";
    }
    if (wantsComplexity) {
        if (wantsSpace && spaceComplexities.count(topic)) {
            response << "Space Complexity: " << spaceComplexities.at(topic) << "\n";
        }
        if (timeComplexities.count(topic)) {
            response << "Time Complexities:\n";
            for (const auto& [op, comp] : timeComplexities.at(topic)) {
                response << "- " << op << ": " << comp << "\n";
            }
        }
    }

    std::string result = response.str();
    return result.empty() ? "I have information about " + topic + ", but not the specific details you requested." : result;
}

bool DSAKnowledge::isDSAQuery(const std::string& query) const {
    if (!initialized) {
        std::cerr << "Warning: DSA knowledge not initialized!" << std::endl;
        return false;
    }
    std::string lowerQuery = hashTable.toLower(query);
    for (const auto& [topic, _] : dsaExplanations) {
        if (lowerQuery.find(hashTable.toLower(topic)) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> DSAKnowledge::getAllTopics() const {
    std::vector<std::string> topics;
    for (const auto& [topic, _] : dsaExplanations) {
        topics.push_back(topic);
    }
    return topics;
}

void DSAKnowledge::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    try {
        json data = json::parse(file);
        for (const auto& section : {"data_structures", "algorithms"}) {
            if (data.contains(section)) {
                for (auto& [topic, expl] : data[section].items()) {
                    dsaExplanations[topic] = expl.get<std::string>();
                }
            }
        }
        if (data.contains("time_complexities")) {
            for (auto& [ds, complexities] : data["time_complexities"].items()) {
                for (auto& [op, comp] : complexities.items()) {
                    timeComplexities[ds][op] = comp.get<std::string>();
                }
            }
        }
        if (data.contains("space_complexities")) {
            for (auto& [ds, comp] : data["space_complexities"].items()) {
                spaceComplexities[ds] = comp.get<std::string>();
            }
        }
        if (data.contains("code_examples")) {
            for (auto& [topic, example] : data["code_examples"].items()) {
                codeExamples[topic] = example.get<std::string>();
            }
        }
        std::cout << "Loaded DSA knowledge from " << filename << std::endl;
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    file.close();
}