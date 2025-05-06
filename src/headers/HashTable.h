#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>

class DSAKnowledge; // Forward declaration

class HashTable {
public:
    // Constructor and Destructor
    HashTable();
    ~HashTable();

    // Core methods
    std::string lookup(const std::string& query);
    std::string get(const std::string& key);
    void addResponse(const std::string& query, const std::string& response);
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;

    // New methods
    void initializeDSAKnowledge();
    bool isHealthy() const;

    //Helper method
    std::string toLower(const std::string& str);
    std::vector<std::string> extractKeywordsFromQuery(const std::string& query, const std::vector<std::string>& stopWords);
    
    // Constants and static data
    static const std::vector<std::string> STOP_WORDS;
private:
    // Constants and static data
    static const std::unordered_set<std::string> DSA_COMPOUND_TERMS;
    
    static const std::unordered_map<std::string, std::string> DSA_SYNONYMS;
    static const std::unordered_set<std::string> IMPORTANT_TERMS;

    // Data structures
    std::vector<std::vector<std::pair<std::string, std::string>>> table;
    std::unordered_map<std::string, std::vector<std::string>> keywords;
    DSAKnowledge* dsaKnowledge;

    // Helper methods
    int hashFunction(const std::string& key);
    int calculateMatchScore(const std::string& key, const std::vector<std::string>& keywords);
    std::string findDirectMatchForCompoundQuery(const std::string& query);
    std::string combineResponsesForConcepts(const std::string& query);
    bool isWholeWord(const std::string& text, const std::string& word);
    std::vector<std::string> splitAndCleanQuery(const std::string& query, const std::string& delimiter);
    std::string cleanWord(const std::string& word);
    std::string trim(const std::string& str);
    std::string normalizeQuery(const std::string& query);
    bool isCompoundQuery(const std::string& query);
};

#endif // HASHTABLE_H