#include "../headers/DecisionTree.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <queue>

using json = nlohmann::json;

// Constructor
DecisionTree::DecisionTree() : root(nullptr) {}

// Destructor
DecisionTree::~DecisionTree() {
    destroyTree(root);
}

// Helper function to delete nodes recursively
void DecisionTree::destroyTree(Node* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

// Add a node to the tree using BFS
void DecisionTree::addNode(const std::string& question, const std::string& leftAnswer, const std::string& rightAnswer) {
    Node* newNode = new Node{question, leftAnswer, rightAnswer, nullptr, nullptr};
    if (!root) {
        root = newNode;
    } else {
        std::queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            Node* current = q.front();
            q.pop();
            if (!current->left) {
                current->left = newNode;
                return;
            } else if (!current->right) {
                current->right = newNode;
                return;
            }
            q.push(current->left);
            q.push(current->right);
        }
    }
}

// Traverse the tree and return the path as a string
std::string DecisionTree::traverse() const {
    if (!root) {
        return "Decision tree is empty.";
    }
    std::string result;
    Node* current = root;
    while (current) {
        result += "Question: " + current->question + "\n";
        if (current->left && !current->leftAnswer.empty()) {
            result += "Answer (Left): " + current->leftAnswer + "\n";
            current = current->left;
        } else if (current->right && !current->rightAnswer.empty()) {
            result += "Answer (Right): " + current->rightAnswer + "\n";
            current = current->right;
        } else {
            break;
        }
    }
    return result;
}

// Check if the tree is initialized
bool DecisionTree::isHealthy() const {
    return root != nullptr;
}

// Load tree from a JSON file
void DecisionTree::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    try {
        json data = json::parse(file);
        if (data.is_array()) {
            for (const auto& nodeData : data) {
                addNode(nodeData["question"].get<std::string>(), 
                        nodeData["left_answer"].get<std::string>(), 
                        nodeData["right_answer"].get<std::string>());
            }
        }
        std::cout << "Loaded decision tree from file" << std::endl;
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    file.close();
}

// Save tree to a JSON file
void DecisionTree::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    json data;
    if (root) {
        std::queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            Node* node = q.front();
            q.pop();
            json nodeData = {
                {"question", node->question},
                {"left_answer", node->leftAnswer},
                {"right_answer", node->rightAnswer}
            };
            data.push_back(nodeData);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }
    file << data.dump(4);
    file.close();
    std::cout << "Saved decision tree to file" << std::endl;
}