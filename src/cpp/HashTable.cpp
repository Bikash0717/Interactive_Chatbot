#include "../headers/HashTable.h"
#include "../headers/DSAKnowledge.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_set>

using json = nlohmann::json;
using namespace std;

const int TABLE_SIZE = 1000;

// Static data members
const unordered_set<string> HashTable::DSA_COMPOUND_TERMS = {
    "linked list", "binary search", "binary tree", "red black tree", 
    "avl tree", "hash table", "hash map", "depth first search", "breadth first search",
    "dfs", "bfs", "depth first", "breadth first", "time complexity", "space complexity",
    "big o", "binary heap", "dynamic programming", "greedy algorithm", "divide and conquer",
    "graph traversal", "topological sort", "minimum spanning tree", "shortest path",
    "trie structure", "segment tree", "b tree", "b+ tree", "dijkstra algorithm",
    "bellman-ford", "floyd-warshall", "a* search", "priority queue", "fibonacci heap",
    "adjacency list", "adjacency matrix"
};

const vector<string> HashTable::STOP_WORDS = {
    "a", "an", "the", "in", "on", "at", "of", "to", "and", "is", "are", "for", 
    "with", "that", "this", "it", "what", "how", "where", "can", "you", "tell", 
    "me", "about", "explain", "describe", "show", "i", "my", "we", "our"
};

const unordered_map<string, string> HashTable::DSA_SYNONYMS = {
    {"linkedlist", "linked list"}, {"ll", "linked list"}, {"list", "linked list"},
    {"arrays", "array"}, {"hashtable", "hash table"}, {"hashmap", "hash table"},
    {"bst", "binary search tree"}, {"binary tree", "tree"}, {"dfs", "depth first search"},
    {"depth-first-search", "depth first search"}, {"depth first", "depth first search"},
    {"depth-first", "depth first search"}, {"bfs", "breadth first search"},
    {"breadth-first-search", "breadth first search"}, {"breadth first", "breadth first search"},
    {"breadth-first", "breadth first search"}, {"complexity", "time complexity"},
    {"runtime", "time complexity"}, {"run time", "runtime"}, {"run-time", "runtime"},
    {"linked-list", "linked list"}, {"avl", "avl tree"}, {"rbt", "red black tree"},
    {"red-black", "red black tree"}, {"dp", "dynamic programming"},
    {"directed acyclic graph", "dag"}, {"dag", "directed acyclic graph"},
    {"pq", "priority queue"}, {"heap", "binary heap"}, {"amortized", "amortized analysis"},
    {"asymptotic", "asymptotic analysis"}, {"sort", "sorting algorithm"},
    {"bubble", "bubble sort"}, {"quick", "quicksort"}, {"merge", "merge sort"},
    {"insert", "insertion sort"}, {"selection", "selection sort"}, {"radix", "radix sort"}
};

const unordered_set<string> HashTable::IMPORTANT_TERMS = {
    "complexity", "algorithm", "data structure", "time", "space", "performance",
    "efficient", "optimization", "tradeoff", "implementation", "recursive", "iterative",
    "memory", "cache", "worst case", "best case", "average case"
};

HashTable::HashTable() {
    table.resize(TABLE_SIZE);
    dsaKnowledge = new DSAKnowledge(*this);
}

HashTable::~HashTable() {
    if (dsaKnowledge) {
        delete dsaKnowledge;
        dsaKnowledge = nullptr;
    }
}

int HashTable::hashFunction(const string& key) {
    unsigned long hash = 5381;                                     //Why 5381? This is a magic number commonly used in the DJB2 algorithm. It’s a prime number chosen by Daniel J. Bernstein (the creator of DJB2) because primes tend to produce better distribution when combined with multiplication and addition operations. Starting with a prime helps reduce the likelihood of collisions (different keys mapping to the same index).     

    for (char c : key) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c (DJB2)
    }
    return hash % TABLE_SIZE;
}

string HashTable::lookup(const string& query) {
    string lowerQuery = normalizeQuery(query);
    int index = hashFunction(lowerQuery);

    // Exact match
    for (const auto& pair : table[index]) {
        if (pair.first == lowerQuery) {
            return pair.second;
            cout << "Exact match";
        }
    }

    // Extract keywords
    vector<string> keywords = extractKeywordsFromQuery(lowerQuery, STOP_WORDS);

    // Compound query handling
    if (isCompoundQuery(lowerQuery)) {
        string combinedQueryResponse = findDirectMatchForCompoundQuery(lowerQuery);
        if (!combinedQueryResponse.empty()) {
            return combinedQueryResponse;
        }
        string combinedResponse = combineResponsesForConcepts(lowerQuery);
        if (!combinedResponse.empty()) {
            return combinedResponse;
        }
        cout << "Compound query handling";
    }

    // Synonym check
    for (const auto& [synonym, canonical] : DSA_SYNONYMS) {
        if (lowerQuery.find(synonym) != string::npos) {
            string result = lookup(canonical); // Recursive call to lookup
            if (result != "Not found") {
                return result;
            }
        }
        cout << "Synonym check";
    }

    // DSA knowledge check
    if (dsaKnowledge && dsaKnowledge->isDSAQuery(lowerQuery)) {
        return dsaKnowledge->generateResponse(lowerQuery);
        cout << "DSA knowledge check";
    }

    // Score-based matching
    unordered_map<string, int> responseScores;
    for (const auto& bucket : table) {
        for (const auto& pair : bucket) {
            int score = calculateMatchScore(pair.first, keywords);
            if (score > 0) {
                responseScores[pair.second] = score;
            }
        }
    }

    if (!responseScores.empty()) {
        auto bestMatch = max_element(responseScores.begin(), responseScores.end(),
            [](const auto& p1, const auto& p2) { return p1.second < p2.second; });
        return bestMatch->first;
        cout << "Score-based matching";
    }

    return "I'm not sure about that. Could you please rephrase your question about data structures and algorithms?";
}

string HashTable::get(const string& key) {
    return lookup(key); // Simplified to use lookup
}

void HashTable::addResponse(const string& query, const string& response) {
    if (query.empty()) {
        throw invalid_argument("Query cannot be empty");
    }
    string lowerQuery = toLower(query);
    int index = hashFunction(lowerQuery);
    table[index].push_back({lowerQuery, response});

    // Extract and store keywords using extractKeywordsFromQuery
    vector<string> keywords = extractKeywordsFromQuery(lowerQuery, STOP_WORDS);
    for (const auto& keyword : keywords) {
        this->keywords[keyword].push_back(lowerQuery);
    }
}

void HashTable::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }
    try {
        json data = json::parse(file);
        for (auto it = data.begin(); it != data.end(); ++it) {
            addResponse(it.key(), it.value().get<string>());
        }
        cout << "Loaded responses from file" << endl;
    } catch (json::parse_error& e) {
        cerr << "JSON parse error: " << e.what() << endl;
    }
    file.close();
}

void HashTable::saveToFile(const string& filename) const {
    json data;
    for (const auto& bucket : table) {
        for (const auto& pair : bucket) {
            data[pair.first] = pair.second;
        }
    }
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file for writing: " << filename << endl;
        return;
    }
    file << data.dump(4);
    file.close();
    cout << "Saved all responses to file" << endl;
}

// Helper Methods
vector<string> HashTable::extractKeywordsFromQuery(
    const string& query, 
    const vector<string>& stopWords
) {
    vector<string> keywords;
    stringstream ss(query);
    string word;
    vector<string> tokens;

    while (ss >> word) {
        word = cleanWord(word);
        if (!word.empty()) {
            tokens.push_back(word);
        }
    }

    for (size_t i = 0; i < tokens.size(); ++i) {
        bool foundCompound = false;
        for (int length = min(size_t(4), tokens.size() - i); length > 1; --length) {
            string compound = tokens[i];
            for (int j = 1; j < length; ++j) {
                compound += " " + tokens[i + j];
            }
            if (DSA_COMPOUND_TERMS.count(compound)) {
                keywords.push_back(compound);
                i += length - 1;
                foundCompound = true;
                break;
            }
        }
        if (!foundCompound && find(stopWords.begin(), stopWords.end(), tokens[i]) == stopWords.end()) {
            keywords.push_back(tokens[i]);
        }
    }
    return keywords;
}

int HashTable::calculateMatchScore(const string& key, const vector<string>& keywords) {   // This function calculates a "match score" for a given key (key) based on how well it matches a list of keywords (keywords). The score is computed by applying various scoring rules depending on the nature of the match.
    if (keywords.empty()) return 0;
    int score = 0;
    string lowerKey = toLower(key);

    for (const auto& keyword : keywords) {
        if (lowerKey.find(keyword) != string::npos) {
            score += 5; // Base match
            if (IMPORTANT_TERMS.count(keyword)) score += 5; // Important term
            if (keyword.find(' ') != string::npos && isWholeWord(lowerKey, keyword)) score += 8; // Compound term
            if (isWholeWord(lowerKey, keyword)) score += 3; // Exact word
            if (lowerKey == keyword) score += 10; // Exact title
        }
    }
    return score;
}

string HashTable::findDirectMatchForCompoundQuery(const string& query) {                //This function attempts to find a direct match for a given query in a hash table (table). It performs case-insensitive matching and supports partial matches if the query appears as a whole word within a key.
    string lowerQuery = toLower(query);
    for (const auto& bucket : table) {
        for (const auto& pair : bucket) {
            string lowerKey = toLower(pair.first);
            if (lowerKey == lowerQuery || (lowerKey.find(lowerQuery) != string::npos && isWholeWord(lowerKey, lowerQuery))) {
                return pair.second;
            }
        }
    }
    return "";
}

string HashTable::combineResponsesForConcepts(const string& query) {            //This function processes a query to extract multiple topics, retrieves responses for each topic from a hash table, and combines these responses into a single cohesive response. It also handles comparisons or transitions between topics based on specific separators.
    string combinedResponse;
    const unordered_map<string, string> separators = {
        {" and ", "\n\nRelating to "}, {" vs ", "\n\nComparing with "}, {" versus ", "\n\nComparing with "},
        {" or ", "\n\nAlternatively, "}, {" compared to ", "\n\nIn comparison with "}, {" over ", "\n\nAs opposed to "}
    };

    string usedSeparator, transitionText;
    vector<string> topics;
    for (const auto& [sep, transition] : separators) {
        if (query.find(sep) != string::npos) {
            topics = splitAndCleanQuery(query, sep);
            usedSeparator = sep;
            transitionText = transition;
            break;
        }
    }

    if (topics.empty()) return "";
    bool isComparison = (usedSeparator.find("vs") != string::npos || usedSeparator.find("versus") != string::npos || usedSeparator.find("compared") != string::npos);

    vector<pair<string, string>> topicResponses;
    for (const auto& topic : topics) {
        string cleanedTopic = cleanWord(topic);
        string lookupTopic = DSA_SYNONYMS.count(cleanedTopic) ? DSA_SYNONYMS.at(cleanedTopic) : cleanedTopic;
        string topicResponse = lookup(lookupTopic);
        if (topicResponse != "Not found") {
            topicResponses.push_back({lookupTopic, topicResponse});
        }
    }

    if (!topicResponses.empty()) {
        if (isComparison) {
            combinedResponse = "Comparison between " + topics[0] + " and " + topics[1] + ":\n\n";
            for (size_t i = 0; i < topicResponses.size(); i++) {
                combinedResponse += to_string(i + 1) + ". " + topicResponses[i].first + ":\n" + topicResponses[i].second + "\n\n";
            }
        } else {
            combinedResponse = "About " + topicResponses[0].first + ":\n" + topicResponses[0].second;
            for (size_t i = 1; i < topicResponses.size(); i++) {
                combinedResponse += transitionText + topicResponses[i].first + ":\n" + topicResponses[i].second;
            }
        }
    }
    return combinedResponse;
}

// Utility Functions
bool HashTable::isWholeWord(const string& text, const string& word) {
    size_t pos = text.find(word);
    if (pos == string::npos) return false;
    bool validStart = (pos == 0 || text[pos-1] == ' ' || ispunct(text[pos-1]));
    bool validEnd = (pos + word.length() == text.length() || text[pos + word.length()] == ' ' || ispunct(text[pos + word.length()]));
    return validStart && validEnd;
}

vector<string> HashTable::splitAndCleanQuery(const string& query, const string& delimiter) {    //This function splits a query string into parts based on a specified delimiter, cleans each part by trimming whitespace, and returns the cleaned parts as a vector of strings.
    vector<string> results;
    size_t start = 0, end = query.find(delimiter);
    while (end != string::npos) {
        string part = trim(query.substr(start, end - start));
        if (!part.empty()) results.push_back(part);
        start = end + delimiter.length();
        end = query.find(delimiter, start);
    }
    string lastPart = trim(query.substr(start));
    if (!lastPart.empty()) results.push_back(lastPart);
    return results;
}

string HashTable::toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

string HashTable::cleanWord(const string& word) {  //ensures that words are normalized for case insensitivity and punctuation is cleaned up while preserving hyphens.
    string cleaned = toLower(word);
    cleaned.erase(remove_if(cleaned.begin(), cleaned.end(), [](char c) { return ispunct(c) && c != '-'; }), cleaned.end());
    return cleaned;
}

string HashTable::trim(const string& str) {    //This function removes leading and trailing whitespace characters from a string and returns the trimmed version of the string.
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, last - first + 1);
}

string HashTable::normalizeQuery(const string& query) {
    string normalized = toLower(query);
    if (normalized.size() >= 2 && normalized.front() == '"' && normalized.back() == '"') {
        normalized = normalized.substr(1, normalized.size() - 2);
    }
    return normalized;
}

bool HashTable::isCompoundQuery(const string& query) {
    const vector<string> indicators = {" and ", " vs ", " versus ", " or ", " compared to ", " over "};
    for (const auto& indicator : indicators) {
        if (query.find(indicator) != string::npos) return true;
    }
    return false;
}

void HashTable::initializeDSAKnowledge() {
    if (!dsaKnowledge) {
        dsaKnowledge = new DSAKnowledge(*this);
    }
}

bool HashTable::isHealthy() const {
    // Implement health check logic here
    return true;
}