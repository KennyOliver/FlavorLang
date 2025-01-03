#!/bin/bash

# Set execute permissions
chmod +x flavorlang-macos-latest

# Move to /usr/local/bin for easier access
sudo mv flavorlang-macos-latest /usr/local/bin/

# Run the executable
flavorlang-macos-latest --about
