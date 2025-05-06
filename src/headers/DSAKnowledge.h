#ifndef DSAKNOWLEDGE_H
#define DSAKNOWLEDGE_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>

class HashTable;  // Forward declaration

class DSAKnowledge {
public:
    // Constructor and Destructor
    DSAKnowledge(HashTable& ht);
    ~DSAKnowledge();

    // Core methods
    std::string generateResponse(const std::string& query) const;
    bool isDSAQuery(const std::string& query) const;
    std::vector<std::string> getAllTopics() const;

private:
    // Initialization
    void initialize();
    void loadFromFile(const std::string& filename);

    // Data storage
    std::map<std::string, std::string> dsaExplanations;
    std::map<std::string, std::map<std::string, std::string>> timeComplexities;
    std::map<std::string, std::string> spaceComplexities;
    std::map<std::string, std::string> codeExamples;

    // Keyword sets for intent detection
    std::unordered_set<std::string> codeKeywords;
    std::unordered_set<std::string> complexityKeywords;

    // Reference to HashTable for integration
    HashTable& hashTable;

    // State
    bool initialized;
};

#endif // DSAKNOWLEDGE_H