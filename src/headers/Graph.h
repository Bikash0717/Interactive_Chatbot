#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <map>

class Graph {
public:
    void addVertex(const std::string& vertex);
    void addEdge(const std::string& from, const std::string& to);
    void bfs(const std::string& start);
    std::string analyze(const std::string& query);
    void addPattern(const std::string& pattern);
    bool isHealthy() const;
    std::vector<std::string> splitQueryIntoWords(const std::string& query);
    void addQueryToGraph(const std::vector<std::string>& words);
    void initialize();
private:
    std::map<std::string, std::vector<std::string>> adjacencyList;
    std::map<std::string, int> patternFrequency;
};

#endif
