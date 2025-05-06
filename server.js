const express = require('express');
const path = require('path');
const { exec } = require('child_process');
const fs = require('fs');
const app = express();
const PORT = process.env.PORT || 5502;

// Middleware for parsing JSON bodies
app.use(express.json());

// Serve static files from the frontend directory
app.use(express.static(path.join(__dirname, 'frontend')));

// Helper function to execute the C++ chatbot executable
function executeChatbot(query) {
  return new Promise((resolve, reject) => {
    // Create a temporary input file for the query
    const tempInputFile = path.join(__dirname, 'temp_query.json');
    fs.writeFileSync(tempInputFile, JSON.stringify({ query }));
    
    // Execute the C++ program
    const chatbotPath = path.join(__dirname, 'build', 'ChatBotServer');
    exec(`"${chatbotPath}" "${tempInputFile}"`, (error, stdout, stderr) => {
      // Clean up temp file
      try {
        fs.unlinkSync(tempInputFile);
      } catch (err) {
        console.error('Error deleting temp file:', err);
      }
      
      if (error) {
        console.error(`Execution error: ${error}`);
        return reject(error);
      }
      
      if (stderr) {
        console.error(`Stderr: ${stderr}`);
      }
      
      console.log('Raw output:', stdout); // Log the raw output for debugging
      
      try {
        // Extract only the JSON part from the output
        const jsonStart = stdout.indexOf('{');
        const jsonEnd = stdout.lastIndexOf('}') + 1;
        const jsonStr = stdout.substring(jsonStart, jsonEnd);
        const result = JSON.parse(jsonStr);
        resolve(result);
      } catch (parseError) {
        console.error('Error parsing chatbot output:', parseError);
        console.error('Raw output:', stdout);
        reject(parseError);
      }
    });
  });
}

// POST endpoint for chatbot queries
app.post('/api/query', async (req, res) => {
  try {
    const { query } = req.body;
    
    if (!query) {
      return res.status(400).json({ error: 'Query parameter is required' });
    }
    
    const result = await executeChatbot(query);
    res.json(result);
  } catch (error) {
    console.error('Error processing query:', error);
    res.status(500).json({ error: 'Failed to process query' });
  }
});

// GET endpoint for health check
app.get('/api/health', (req, res) => {
  // Execute the chatbot with a health check command
  executeChatbot('__health_check__')
    .then(result => {
      res.json({ status: 'healthy', ...result });
    })
    .catch(error => {
      console.error('Health check failed:', error);
      console.error('Error details:', error);
      res.status(500).json({ status: 'unhealthy', error: error.message });
    });
});

// Start the server
app.listen(PORT, (err) => {
  if (err) {
    console.error('Failed to start server:', err);
  } else {
    console.log(`Server running on port ${PORT}`);
  }
});
