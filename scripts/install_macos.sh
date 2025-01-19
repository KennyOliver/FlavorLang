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
echo "  flavor --about"
echo ""
echo "Note: On macOS, the executable may be blocked by Gatekeeper."
echo "To allow execution, go to System Settings > Privacy & Security,"
echo "locate the blocked FlavorLang binary, and click 'Allow Anyway.'"
echo "Afterward, run the command again, and select 'Open Anyway' on the security prompt."
