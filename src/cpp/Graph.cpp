#include "../headers/Graph.h"
#include <algorithm>
#include <queue>
#include <iostream>
#include <sstream>
#include <cctype>
#include <unordered_set>

// DSA-related keywords
const std::unordered_set<std::string> DSA_KEYWORDS = {
    "algorithm", "data", "structure", "array", "list", "linked", "linkedlist", "stack", "queue", "tree",
    "graph", "hash", "sort", "search", "complexity", "binary", "heap", "dfs", "bfs",
    "dynamic", "programming", "greedy", "backtracking", "recursion", "iteration", "time", "space",
    "big", "notation", "avl", "red", "black", "bst", "traversal", "inorder", "preorder", "postorder",
    "insertion", "deletion", "access", "push", "pop", "enqueue", "dequeue", "merge", "quick", "bubble",
    "selection", "dijkstra", "kruskal", "prim", "bellman", "ford", "floyd", "warshall", "hashmap",
    "hashtable", "hashing", "collision", "chaining", "probing", "linear", "quadratic", "double",
    "runtime", "performance", "efficiency", "memory", "amortized", "worst", "average", "best", "case",
    "o", "log", "exponential", "polynomial", "factorial", "constant", "bigo", "omega", "theta",
    "avl", "rbt", "trie", "b-tree", "btree", "segment", "fenwick", "disjoint", "set", "union", "find",
    "adjacency", "matrix", "mst", "topological", "dag", "scc", "strongly", "connected", "component",
    "dp", "memoization", "tabulation", "knapsack", "lcs", "edit", "distance", "substring", "subsequence"
};

// Conversational keywords
const std::unordered_set<std::string> CONVERSATIONAL_KEYWORDS = {
    "hello", "hi", "hey", "greetings", "help", "thanks", "thank", "bye", "goodbye",
    "how", "what", "when", "where", "why", "explain", "tell", "show", "describe",
    "can", "could", "would", "should", "please", "need", "want", "looking", "for"
};

void Graph::addVertex(const std::string& vertex) {
    if (adjacencyList.find(vertex) == adjacencyList.end()) {
        adjacencyList[vertex] = std::vector<std::string>();
        patternFrequency[vertex] = 0;
    }
}

void Graph::addEdge(const std::string& from, const std::string& to) {
    addVertex(from);
    addVertex(to);
    if (std::find(adjacencyList[from].begin(), adjacencyList[from].end(), to) == adjacencyList[from].end()) {
        adjacencyList[from].push_back(to);
    }
}

void Graph::bfs(const std::string& start) {
    if (adjacencyList.find(start) == adjacencyList.end()) {
        std::cout << "Start vertex not found in the graph!" << std::endl;
        return;
    }
    std::queue<std::string> q;
    std::map<std::string, bool> visited;
    q.push(start);
    visited[start] = true;
    while (!q.empty()) {
        std::string current = q.front();
        q.pop();
        std::cout << current << " ";
        for (const std::string& neighbor : adjacencyList[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    std::cout << std::endl;
}

std::string Graph::analyze(const std::string& query) {
    std::vector<std::string> words = splitQueryIntoWords(query);
    std::vector<std::pair<std::string, int>> foundPatterns;

    // Find compound terms
    for (size_t i = 0; i < words.size(); i++) {
        for (int len = std::min(3, static_cast<int>(words.size() - i)); len > 0; len--) {
            std::string compound;
            for (int j = 0; j < len; j++) {
                compound += (j > 0 ? " " : "") + words[i + j];
            }
            if (patternFrequency.find(compound) != patternFrequency.end()) {
                foundPatterns.push_back({compound, patternFrequency[compound]});
                i += len - 1;
                break;
            }
        }
    }

    // Check for DSA and conversational keywords
    bool isDSAQuery = false;
    bool isConversational = false;
    for (const auto& pattern : foundPatterns) {
        if (DSA_KEYWORDS.count(pattern.first)) isDSAQuery = true;
        if (CONVERSATIONAL_KEYWORDS.count(pattern.first)) isConversational = true;
    }
    for (const auto& word : words) {
        if (DSA_KEYWORDS.count(word)) isDSAQuery = true;
        if (CONVERSATIONAL_KEYWORDS.count(word)) isConversational = true;
    }

    // Build response
    std::ostringstream responseStream;
    if (!foundPatterns.empty()) {
        if (isDSAQuery) {
            responseStream << "Found DSA-related patterns: ";
        } else if (isConversational) {
            responseStream << "Found conversational patterns: ";
        } else {
            responseStream << "Found patterns: ";
        }
        for (size_t i = 0; i < foundPatterns.size(); i++) {
            if (i > 0) responseStream << ", ";
            responseStream << "'" << foundPatterns[i].first << "' (" << foundPatterns[i].second << " occurrences)";
        }
    } else {
        if (isDSAQuery) {
            responseStream << "Recognized as a DSA-related query, but no specific patterns found.";
        } else if (isConversational) {
            responseStream << "Recognized as a conversational query.";
        } else {
            responseStream << "No familiar patterns found in your query. Learning from this interaction...";
        }
    }

    addQueryToGraph(words);
    return responseStream.str();
}

void Graph::addPattern(const std::string& pattern) {
    if (pattern.empty()) return;
    std::vector<std::string> words = splitQueryIntoWords(pattern);
    if (pattern.find(' ') != std::string::npos) {
        addVertex(pattern);
        patternFrequency[pattern]++;
    }
    for (size_t i = 0; i < words.size(); i++) {
        addVertex(words[i]);
        patternFrequency[words[i]]++;
        if (i > 0) {
            addEdge(words[i-1], words[i]);
            if (pattern.find(' ') != std::string::npos) {
                addEdge(words[i], pattern);
                addEdge(pattern, words[i]);
            }
        }
    }
}

bool Graph::isHealthy() const {
    return !adjacencyList.empty();
}

std::vector<std::string> Graph::splitQueryIntoWords(const std::string& query) {
    std::vector<std::string> words;
    std::istringstream iss(query);
    std::string word;
    while (iss >> word) {
        word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (!word.empty()) words.push_back(word);
    }
    return words;
}

void Graph::addQueryToGraph(const std::vector<std::string>& words) {
    for (size_t i = 0; i < words.size(); i++) {
        addVertex(words[i]);
        patternFrequency[words[i]]++;
        if (i > 0) addEdge(words[i-1], words[i]);
    }
    for (size_t i = 0; i < words.size(); i++) {
        for (int len = std::min(3, static_cast<int>(words.size() - i)); len > 1; len--) {
            std::string compound;
            for (int j = 0; j < len; j++) {
                compound += (j > 0 ? " " : "") + words[i + j];
            }
            addPattern(compound);
        }
    }
}

void Graph::initialize() {
    // Conversational patterns
    addPattern("hello");
    addPattern("hi");
    addPattern("hey");
    addPattern("help");
    addPattern("thank you");
    addPattern("thanks");
    addPattern("bye");
    addPattern("goodbye");

    // DSA patterns
    addPattern("linked list");
    addPattern("binary tree");
    addPattern("hash table");
    addPattern("quick sort");
    addPattern("depth first search");
    addPattern("breadth first search");
    addPattern("array");
    addPattern("stack");
    addPattern("queue");
    addPattern("binary search");
    addPattern("time complexity");
    addPattern("space complexity");
    addPattern("linkedlist");
    addPattern("hashtable");
    addPattern("binarytree");
    addPattern("binarysearch");
    addPattern("bst");
    addPattern("avl");
    addPattern("dfs");
    addPattern("bfs");
    addPattern("dag");
    addPattern("scc");
    addPattern("dp");
    addPattern("runtime");
    addPattern("performance");
    addPattern("efficiency");
    addPattern("big o");
    addPattern("binary search tree");
    addPattern("red black tree");
    addPattern("b-tree");
    addPattern("trie");
    addPattern("segment tree");
    addPattern("fenwick tree");
    addPattern("disjoint set");

    std::cout << "Graph initialized with basic patterns" << std::endl;
}