# ChatBot using DSA

A chatbot implementation using various data structures and algorithms (DSA).

## Components

- **HashTable**: For quick response lookup
- **DecisionTree**: For conversational flow management
- **Graph**: For analyzing query relationships and patterns

## Training the Chatbot

The chatbot can be trained using the `TrainChatbot` application. This enhances the chatbot's knowledge base and improves its response quality.

### Training Data Formats

Two formats are supported:

1. **Text Format** (.txt):
   ```
   Q:What is machine learning?
   A:Machine learning is a subset of artificial intelligence...

   Q:How do neural networks work?
   A:Neural networks are computing systems inspired by...
   ```

2. **JSON Format** (.json):
   ```json
   [
     {
       "query": "What are data structures?",
       "response": "Data structures are specialized formats..."
     },
     {
       "query": "What is a linked list?",
       "response": "A linked list is a linear data structure..."
     }
   ]
   ```

### Training Methods

#### 1. Loading from Files

```bash
# Train from a text file
./TrainChatbot --file /path/to/training_data.txt

# Train from a JSON file
./TrainChatbot --json /path/to/training_data.json
```

#### 2. Interactive Training

```bash
./TrainChatbot --interactive
```

Follow the prompts to add question-answer pairs directly.

#### 3. Analyzing Training Data

```bash
# Load and analyze data
./TrainChatbot --file /path/to/training_data.txt --analyze

# Show statistics about training data
./TrainChatbot --json /path/to/training_data.json --stats
```

#### 4. Saving Trained Models

```bash
# Train and save the model
./TrainChatbot --file /path/to/training_data.txt --save /path/to/model_directory
```

### Training Best Practices

1. **Start with Quality Data**: Begin with accurate, well-formatted training data
2. **Diversify Questions**: Include variations of the same question to improve recognition
3. **Include Domain-Specific Knowledge**: Add specialized knowledge relevant to your use case
4. **Balance Topics**: Ensure a good distribution of topics to avoid bias
5. **Regularly Update**: Add new information as it becomes available
6. **Analyze Patterns**: Use the `--analyze` flag to identify gaps in your training data

### Example Training Workflow

1. Create initial training data file with common queries
2. Train the chatbot:
   ```bash
   ./TrainChatbot --file data/initial_training.txt --save models/baseline
   ```
3. Analyze the training data:
   ```bash
   ./TrainChatbot --file data/initial_training.txt --analyze --stats
   ```
4. Identify gaps and add more training data
5. Run interactive training to add specific responses:
   ```bash
   ./TrainChatbot --interactive --save models/enhanced
   ```
6. Test the chatbot with various queries and iterate the training process

## Implementation Details

### HashTable Training

The HashTable component is trained by adding query-response pairs to an internal map. During training:

1. Queries are normalized (converted to lowercase)
2. Keywords are extracted to improve partial matching
3. Multiple similar queries can map to the same response

### DecisionTree Training

The DecisionTree builds a conversation flow by:

1. Grouping queries by detected topics/intents
2. Creating decision nodes for each topic
3. Building a tree structure to guide conversations

### Graph Training

The Graph component creates a network of related queries by:

1. Adding each unique query as a vertex
2. Creating edges between semantically similar queries
3. Using shared keywords to determine relationships

## Building From Source

To build the training module:

```bash
cd /home/manish/Documents/Projects/ChatBotusingDSA
mkdir build && cd build
cmake ..
make TrainChatbot
```

## Dependencies

- C++17 or higher
- nlohmann/json for JSON parsing
- filesystem (C++17 standard library)
