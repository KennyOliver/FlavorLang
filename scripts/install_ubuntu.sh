#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Set execute permissions
chmod +x flavor

# Move to /usr/local/bin for easier access
echo "Moving flavor to /usr/local/bin/"
sudo mv flavor /usr/local/bin/

# Show how to run the executable
echo "To run the FlavorLang interpreter:"
echo "flavor --about"
