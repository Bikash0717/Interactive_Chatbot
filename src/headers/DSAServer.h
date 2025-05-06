#ifndef DSAServer_H
#define DSAServer_H

#include <string>
#include <chrono>
#include <mutex>
#include <unordered_map>

class DSAServer {
public:
    void processQuery(const std::string& inputFile);
};

void startServer(); // Kept for backward compatibility

#endif
