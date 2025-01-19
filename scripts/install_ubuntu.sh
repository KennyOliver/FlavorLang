#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Move the binary
echo "=========================================="
echo "Step 1: Moving FlavorLang binary to /usr/local/bin/"
echo "=========================================="
chmod +x flavor
sudo mv flavor /usr/local/bin/
echo "[SUCCESS] FlavorLang binary moved successfully."
echo ""

# Install autocompletion scripts
USER_SHELL=$(basename "$SHELL")

echo "=========================================="
echo "Step 2: Installing autocomplete script for $USER_SHELL"
echo "=========================================="

if [[ "$USER_SHELL" == "bash" ]]; then
    COMPLETION_DIR="/etc/bash_completion.d"
    COMPLETION_FILE="$COMPLETION_DIR/flavor_completion.bash"

    sudo mkdir -p "$COMPLETION_DIR"
    sudo cp autocomplete/bash_completion.sh "$COMPLETION_FILE"

    echo "[SUCCESS] Bash completion script installed to:"
    echo "   $COMPLETION_FILE"
    echo ""
    echo "This script will automatically load when you start a new shell."
    echo "If it doesn't, ensure that bash completion is enabled on your system."
    echo ""

elif [[ "$USER_SHELL" == "zsh" ]]; then
    COMPLETION_DIR="$HOME/.zsh/completion"
    COMPLETION_FILE="$COMPLETION_DIR/_flavor"

    mkdir -p "$COMPLETION_DIR"
    cp autocomplete/zsh_completion.sh "$COMPLETION_FILE"

    echo "[SUCCESS] Zsh completion script installed to:"
    echo "   $COMPLETION_FILE"
    echo ""
    echo "Add the following line to your ~/.zshrc (if not already present):"
    echo "   fpath+=$COMPLETION_DIR"
    echo "Then run:"
    echo "   source ~/.zshrc"
    echo "Or restart your shell to enable it."
    echo ""

else
    echo "[FAIL] Autocomplete installation is not supported for shell: $USER_SHELL"
    echo ""
fi

echo "=========================================="
echo "Step 3: Installation Complete"
echo "=========================================="
echo "To run the FlavorLang interpreter, use:"
echo "   flavor --about"
echo ""
