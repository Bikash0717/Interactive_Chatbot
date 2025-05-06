#ifndef DECISIONTREE_H
#define DECISIONTREE_H

#include <string>
#include <queue>

class DecisionTree {
private:
    struct Node {
        std::string question;
        std::string leftAnswer;
        std::string rightAnswer;
        Node* left;
        Node* right;
    };
    Node* root;
    void destroyTree(Node* node);

public:
    DecisionTree();
    ~DecisionTree();
    void addNode(const std::string& question, const std::string& leftAnswer, const std::string& rightAnswer);
    std::string traverse() const;
    void initialize();
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    bool isHealthy() const;
};

#endif // DECISIONTREE_H