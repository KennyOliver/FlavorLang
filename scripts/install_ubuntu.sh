#!/bin/bash

# Set execute permissions
chmod +x flavorlang-ubuntu-latest

# Move to /usr/local/bin for easier access
sudo mv flavorlang-ubuntu-latest /usr/local/bin/

# Run the executable
flavorlang-ubuntu-latest --about
