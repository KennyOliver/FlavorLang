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
    COMPLETION_DIR="$HOME/.bash_completion.d"
    COMPLETION_FILE="$COMPLETION_DIR/flavor_completion.bash"

    mkdir -p "$COMPLETION_DIR"
    cp autocomplete/bash_completion.sh "$COMPLETION_FILE"

    echo "[SUCCESS] Bash completion script installed to:"
    echo "   $COMPLETION_FILE"
    echo ""
    echo "To activate it, run:"
    echo "   source ~/.bashrc"
    echo "Or restart your shell."
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
    echo "Or restart your shell."
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

echo "[WARNING]  Note: On macOS, the executable may be blocked by Gatekeeper."
echo "To allow execution:"
echo "  1. Open System Settings > Privacy & Security."
echo "  2. Locate the blocked FlavorLang binary and click 'Allow Anyway.'"
echo "  3. Run the command again and select 'Open Anyway' on the security prompt."
echo ""
