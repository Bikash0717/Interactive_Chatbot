#!/bin/bash

# Navigate to the project root directory
cd "$(dirname "$0")"

# Check if the PID file exists
if [ -f .server.pid ]; then
  # Read the PID from the file
  server_pid=$(cat .server.pid)
  
  # Check if the process is running
  if ps -p $server_pid > /dev/null; then
    echo "Stopping server with PID: $server_pid"
    kill -15 $server_pid
    sleep 1
    
    # Force kill if still running
    if ps -p $server_pid > /dev/null; then
      echo "Force stopping server with PID: $server_pid"
      kill -9 $server_pid
    fi
    
    echo "Server stopped successfully"
  else
    echo "Server is not running with PID: $server_pid"
  fi
  
  # Remove the PID file
  rm .server.pid
else
  # Try to find any Node.js process running on port 5502
  port_pid=$(lsof -t -i:5502)
  if [ -n "$port_pid" ]; then
    echo "Found server running on port 5502 with PID: $port_pid"
    echo "Stopping server..."
    kill -9 $port_pid
    echo "Server stopped successfully"
  else
    echo "No server process found running on port 5502"
  fi
fi

echo "Server shutdown complete"
