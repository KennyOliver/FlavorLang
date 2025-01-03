#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Rename file to flavorlang
echo "Renaming flavorlang-ubuntu-latest to flavorlang"
mv flavorlang-ubuntu-latest flavorlang

# Set execute permissions
chmod +x flavorlang

# Move to /usr/local/bin for easier access
echo "Moving flavorlang to /usr/local/bin/"
sudo mv flavorlang /usr/local/bin/

# Run the executable
echo "Running FlavorLang interpreter..."
flavorlang --about
