#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Set execute permissions
chmod +x flavor

# Move to /usr/local/bin for easier access
echo "Moving flavor to /usr/local/bin/"
sudo mv flavor /usr/local/bin/

# Run the executable
echo "Running FlavorLang interpreter..."
flavor --about
