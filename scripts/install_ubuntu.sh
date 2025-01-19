#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Move the binary
echo "Moving flavor binary to /usr/local/bin/"
chmod +x flavor
sudo mv flavor /usr/local/bin/

# Install autocompletion scripts
USER_SHELL=$(basename "$SHELL")

echo "Installing autocomplete script for $USER_SHELL..."

if [[ "$USER_SHELL" == "bash" ]]; then
    COMPLETION_DIR="/etc/bash_completion.d"
    COMPLETION_FILE="$COMPLETION_DIR/flavor_completion.bash"

    sudo mkdir -p "$COMPLETION_DIR"
    sudo cp autocomplete/bash_completion.bash "$COMPLETION_FILE"

    echo "Bash completion script installed to $COMPLETION_FILE."
    echo "The script will automatically load when you start a new shell."
    echo "If it doesn't, ensure that bash completion is enabled on your system."

elif [[ "$USER_SHELL" == "zsh" ]]; then
    COMPLETION_DIR="$HOME/.zsh/completion"
    COMPLETION_FILE="$COMPLETION_DIR/_flavor"

    mkdir -p "$COMPLETION_DIR"
    cp autocomplete/zsh_completion.zsh "$COMPLETION_FILE"

    echo "Zsh completion script installed to $COMPLETION_FILE."
    echo "Add the following line to your ~/.zshrc if not already present:"
    echo "fpath+=$COMPLETION_DIR"
    echo "Then run 'source ~/.zshrc' or restart your shell to enable it."

else
    echo "Autocomplete installation not supported for shell: $USER_SHELL"
fi

echo "Installation complete!"

# Show how to run the executable
echo "To run the FlavorLang interpreter:"
echo "  flavor --about"
