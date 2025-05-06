#!/bin/bash

# Function to check if a port is in use
port_in_use() {
  lsof -i :$1 > /dev/null
  return $?
}

# Check if port 5502 is in use and either kill it or use a different port
if port_in_use 5502; then
  echo "Port 5502 is already in use."
  echo "Choose an option:"
  echo "1. Kill the process using port 5502 (may require sudo)"
  echo "2. Use a different port"
  read -p "Enter your choice (1/2): " choice
  
  if [ "$choice" = "1" ]; then
    echo "Attempting to kill process on port 5502..."
    PID=$(lsof -t -i:5502)
    if [ -n "$PID" ]; then
      kill $PID 2>/dev/null || sudo kill $PID || echo "Failed to kill process. Trying different port."
      sleep 1
      if port_in_use 5502; then
        echo "Process still running. Using a different port."
        sed -i 's/const PORT = process.env.PORT || 5502/const PORT = process.env.PORT || 5503/' server.js
        echo "Modified server.js to use port 5503"
      else
        echo "Successfully killed process."
      fi
    fi
  else
    # Modify server.js to use port 5503
    sed -i 's/const PORT = process.env.PORT || 5502/const PORT = process.env.PORT || 5503/' server.js
    echo "Modified server.js to use port 5503"
  fi
fi

# Create required JSON files if they don't exist
echo "Checking for required JSON files..."

if [ ! -f "responses.json" ]; then
  echo "Creating responses.json..."
  cat > responses.json << EOF
{
  "what is a linked list": "A linked list is a linear data structure where elements are stored in nodes. Each node contains data and a reference to the next node.",
  "what is an array": "An array is a collection of elements stored at contiguous memory locations, accessible using indices.",
  "what is a binary tree": "A binary tree is a tree data structure where each node has at most two children, referred to as left and right child."
}
EOF
fi

if [ ! -f "decision_tree.json" ]; then
  echo "Creating decision_tree.json..."
  cat > decision_tree.json << EOF
[
  {
    "question": "Are you looking for information about a specific data structure?",
    "left_answer": "Yes, I can help with that. What data structure are you interested in?",
    "right_answer": "Would you like to know about algorithms instead?"
  },
  {
    "question": "Which data structure are you interested in?",
    "left_answer": "Linked lists are linear data structures where elements are stored in nodes.",
    "right_answer": "Arrays are collections of elements stored at contiguous memory locations."
  }
]
EOF
fi

if [ ! -f "dsa_knowledge.json" ]; then
  echo "Creating dsa_knowledge.json..."
  cat > dsa_knowledge.json << EOF
{
  "data_structures": {
    "linked list": "A linked list is a linear data structure where elements are stored in nodes. Each node contains data and a reference to the next node.",
    "array": "An array is a collection of elements stored at contiguous memory locations, accessible using indices."
  },
  "time_complexities": {
    "linked list": {
      "insert_front": "O(1)",
      "insert_end": "O(1) with tail pointer, O(n) without",
      "delete": "O(1) if node given, O(n) to find node",
      "search": "O(n)"
    },
    "array": {
      "access": "O(1)",
      "insert": "O(n)",
      "delete": "O(n)",
      "search": "O(n), O(log n) if sorted"
    }
  },
  "space_complexities": {
    "linked list": "O(n)",
    "array": "O(n)"
  },
  "code_examples": {
    "linked list": "struct Node {\\n    int data;\\n    Node* next;\\n    Node(int val) : data(val), next(nullptr) {}\\n};"
  }
}
EOF
fi

# Start the Express.js server in the background
echo "Starting Express.js server..."
node server.js &
SERVER_PID=$!

# Compile the C++ program
echo "Compiling C++ program..."
mkdir -p build
g++ -std=c++17 \
    src/cpp/main.cpp \
    src/cpp/DSAServer.cpp \
    src/cpp/DecisionTree.cpp \
    src/cpp/Graph.cpp \
    src/cpp/HashTable.cpp \
    src/cpp/DSAKnowledge.cpp \
    src/cpp/Trie.cpp \
    src/cpp/PriorityQueue.cpp \
    -o build/dsa_knowledge

echo "C++ program compiled successfully."

# Create a sample query file if it doesn't exist
if [ ! -f query.json ]; then
  echo '{"query": "What is a linked list?"}' > query.json
  echo "Created sample query.json file"
fi

# Run the C++ program with the sample query
echo "Running C++ program with query.json..."
./build/dsa_knowledge query.json

echo ""
echo "Server is running in the background. You can access it at http://localhost:5502"
echo "To test the API, try: curl -X POST -H 'Content-Type: application/json' -d '{\"query\":\"What is a linked list?\"}' http://localhost:5502/api/query"
echo ""

# Wait for user to press a key before exiting
read -p "Press any key to stop the server and exit..." -n1 -s
echo ""

# Kill the Express.js server
kill $SERVER_PID
echo "Server stopped. Exiting."

# Navigate to the project root directory
cd "$(dirname "$0")"

# Kill any existing Node.js process on port 5502
echo "Checking for existing server processes..."
pid=$(lsof -t -i:5502)
if [ -n "$pid" ]; then
  echo "Killing existing server process on port 5502..."
  kill -9 $pid
fi

# Ensure the C++ executable is built
echo "Checking if C++ executable needs to be built..."
if [ ! -f "./build/ChatBotServer" ] || [ ./src/cpp/DSAServer.cpp -nt ./build/ChatBotServer ]; then
  echo "Building C++ executable..."
  cd build && make
  cd ..
fi

# Start the server in the background
echo "Starting Express.js server..."
node server.js > server.log 2>&1 &
server_pid=$!

# Save the PID to a file for later use by stop.sh
echo $server_pid > .server.pid

echo "Server started with PID: $server_pid"
echo "Server is running at http://localhost:5502"
echo "To stop the server, run: ./stop.sh"

# Wait for the server to start
sleep 2

# Open the frontend in the default browser (optional)
echo "Opening frontend in browser..."
xdg-open http://localhost:5502
