document.addEventListener('DOMContentLoaded', function() {
    const userQueryInput = document.getElementById('userQuery');
    const sendButton = document.getElementById('sendButton');
    const responseDiv = document.getElementById('response');
    
    // Server URL
    const serverUrl = '/api';
    
    // Function to add message to the chat
    function addMessage(message, isUser = false) {
        const messageDiv = document.createElement('div');
        messageDiv.className = `message ${isUser ? 'user' : 'bot'}`;
        
        const contentDiv = document.createElement('div');
        contentDiv.className = 'message-content';
        contentDiv.textContent = message;
        
        const timestamp = document.createElement('span');
        timestamp.className = 'timestamp';
        timestamp.textContent = 'Now';
        
        messageDiv.appendChild(contentDiv);
        messageDiv.appendChild(timestamp);
        
        responseDiv.appendChild(messageDiv);
        
        // Scroll to bottom
        responseDiv.scrollTop = responseDiv.scrollHeight;
    }
    
    // Function to show loading indicator
    function showLoading() {
        // No changes to the interface
    }

    // Function to hide loading indicator
    function hideLoading() {
        // No changes to the interface
    }
    
    // Function to handle errors
    function handleError(error) {
        const messageDiv = document.createElement('div');
        messageDiv.className = 'message bot';
        
        const contentDiv = document.createElement('div');
        contentDiv.className = 'message-content error-content';
        contentDiv.textContent = `Error: ${error.message || 'Failed to communicate with the server'}`;
        
        const timestamp = document.createElement('span');
        timestamp.className = 'timestamp';
        timestamp.textContent = 'Now';
        
        messageDiv.appendChild(contentDiv);
        messageDiv.appendChild(timestamp);
        
        responseDiv.appendChild(messageDiv);
        responseDiv.scrollTop = responseDiv.scrollHeight;
    }
    
    // Revised function to send query to server
    async function sendQuery(query) {
        try {
            showLoading();
            
            // Send an OPTIONS request for CORS preflight
            const preflightResponse = await fetch(`${serverUrl}/query`, {
                method: 'OPTIONS',
                headers: {
                    'Access-Control-Request-Method': 'POST',
                    'Access-Control-Request-Headers': 'Content-Type',
                    'Origin': window.location.origin
                }
            });

            if (!preflightResponse.ok && preflightResponse.status !== 204) {
                throw new Error('CORS preflight request failed');
            }
            
            // Send the actual POST request
            const response = await fetch(`${serverUrl}/query`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Accept': 'application/json',
                    'Origin': window.location.origin
                },
                mode: 'cors',
                credentials: 'omit',
                body: JSON.stringify({ query })
            });
            
            if (!response.ok) {
                throw new Error(`Server responded with status: ${response.status}`);
            }
            
            const data = await response.json();
            
            // Determine the response message based on server data
            // let responseMessage = '';
            
            // if (data.hash_table_result && data.hash_table_result !== 'Not found' && 
            //     data.hash_table_result !== "I'm not sure how to respond to that. Could you please rephrase your question?") {
            //     responseMessage = data.hash_table_result;
            //     console.log('Using hash table result:', responseMessage);
            // } else if (data.graph_analysis && data.graph_analysis !== 'No familiar patterns found in your query. Learning from this interaction...') {
            //     responseMessage = data.graph_analysis;
            //     console.log('Using graph analysis:', responseMessage);
            // } else if (data.decision_tree_response && data.decision_tree_response !== 'Are you looking for DSA help?') {
            //     responseMessage = data.decision_tree_response;
            //     console.log('Using decision tree response:', responseMessage);
            // } else if (data.query_received && isDSARelated(data.query_received)) {
            //     responseMessage = getDSAResponse(data.query_received);
            //     console.log('Using custom DSA response:', responseMessage);
            // } else if (data.decision_tree_response) {
            //     responseMessage = data.decision_tree_response;
            //     console.log('Using decision tree as fallback:', responseMessage);
            // } else {
            //     responseMessage = 'Sorry, I could not process your request at this time.';
            //     console.log('Using default fallback message');
            // }

            
            console.log('Full response data:', data);
            
            // Check for hash table result first (this is the main DSA knowledge source)
            if (data.hash_table_result && data.hash_table_result !== 'Not found' && 
                data.hash_table_result !== "I'm not sure how to respond to that. Could you please rephrase your question?") {
                responseMessage = data.hash_table_result;
                console.log('Using hash table result:', responseMessage);
            }
            // If no hash table result, check graph analysis
            else if (data.graph_analysis && data.graph_analysis !== 'No familiar patterns found in your query. Learning from this interaction...') {
                responseMessage = data.graph_analysis;
                console.log('Using graph analysis:', responseMessage);
            }
            // If no graph analysis, check decision tree response
            else if (data.decision_tree_response && data.decision_tree_response !== 'Are you looking for DSA help?') {
                responseMessage = data.decision_tree_response;
                console.log('Using decision tree response:', responseMessage);
            }
            // If decision tree asks if looking for DSA help and query is DSA-related
            else if (data.query_received && isDSARelated(data.query_received)) {
                responseMessage = getDSAResponse(data.query_received);
                console.log('Using custom DSA response:', responseMessage);
            }
            // Use decision tree as fallback
            else if (data.decision_tree_response) {
                responseMessage = data.decision_tree_response;
                console.log('Using decision tree as fallback:', responseMessage);
            }
            // If all else fails, use a fallback message
            else {
                responseMessage = 'I\'m still learning about this topic. Could you provide more details or ask a different question?';
                console.log('Using fallback message');
            }
            // Add the determined message to the chat
            addMessage(responseMessage, false);
            
        } catch (error) {
            console.error('Error:', error);
            handleError(error);
        } finally {
            hideLoading();
        }
    }
    
    // Event listener for send button
    sendButton.addEventListener('click', function() {
        const query = userQueryInput.value.trim();
        
        if (query) {
            addMessage(query, true);
            sendQuery(query);
            userQueryInput.value = '';
        }
    });
    
    // Event listener for Enter key
    userQueryInput.addEventListener('keypress', function(event) {
        if (event.key === 'Enter') {
            const query = userQueryInput.value.trim();
            
            if (query) {
                addMessage(query, true);
                sendQuery(query);
                userQueryInput.value = '';
            }
        }
    });
    
    // Check server health on load with proper CORS handling
    fetch(`${serverUrl}/health`, {
        method: 'GET',
        headers: {
            'Accept': 'application/json',
            'Origin': window.location.origin
        },
        mode: 'cors',
        credentials: 'omit'
    })
        .then(response => {
            if (!response.ok) {
                throw new Error('Server health check failed');
            }
            return response.json();
        })
        .then(data => {
            console.log('Server health:', data);
        })
        .catch(error => {
            console.error('Server health check error:', error);
            addMessage('Warning: Could not connect to the server. The chatbot might not work properly.', false);
        });
});