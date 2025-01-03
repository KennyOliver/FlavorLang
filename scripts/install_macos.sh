#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Set execute permissions
chmod +x flavorlang

# Move to /usr/local/bin for easier access
echo "Moving flavorlang to /usr/local/bin/"
sudo mv flavorlang /usr/local/bin/

# Run the executable
echo "Running FlavorLang interpreter..."
flavorlang --about
