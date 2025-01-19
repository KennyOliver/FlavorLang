#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Colors and formatting
RESET="\033[0m"
BOLD="\033[1m"
GREEN="\033[32m"
RED="\033[31m"
YELLOW="\033[33m"

echo -e "${BOLD}==========================================${RESET}"
echo -e "${BOLD}Step 1: Moving FlavorLang binary to /usr/local/bin/${RESET}"
echo -e "${BOLD}==========================================${RESET}"
chmod +x flavor
sudo mv flavor /usr/local/bin/
echo -e "[${GREEN}${BOLD}SUCCESS${RESET}] FlavorLang binary moved successfully."
echo ""

USER_SHELL=$(basename "$SHELL")

echo -e "${BOLD}==========================================${RESET}"
echo -e "${BOLD}Step 2: Installing autocomplete script for $USER_SHELL${RESET}"
echo -e "${BOLD}==========================================${RESET}"

if [[ "$USER_SHELL" == "bash" ]]; then
    COMPLETION_DIR="/etc/bash_completion.d"
    COMPLETION_FILE="$COMPLETION_DIR/flavor_completion.bash"

    sudo mkdir -p "$COMPLETION_DIR"
    sudo cp autocomplete/bash_completion.sh "$COMPLETION_FILE"

    echo -e "[${GREEN}${BOLD}SUCCESS${RESET}] Bash completion script installed to:"
    echo "   $COMPLETION_FILE"
    echo ""

    if ! command -v complete &> /dev/null; then
        echo -e "[${YELLOW}${BOLD}WARNING${RESET}] Bash Completion is not enabled on your system."
        echo "Please install it using your package manager. For example:"
        echo "  sudo apt install bash-completion  # On Ubuntu/Debian"
        echo ""
    else
        echo "The script will load automatically when you start a new shell."
        echo "If it doesn't, ensure that Bash Completion is enabled on your system."
    fi
    echo ""

elif [[ "$USER_SHELL" == "zsh" ]]; then
    COMPLETION_DIR="$HOME/.zsh/completion"
    COMPLETION_FILE="$COMPLETION_DIR/_flavor"

    mkdir -p "$COMPLETION_DIR"
    cp autocomplete/zsh_completion.sh "$COMPLETION_FILE"

    if ! grep -q "fpath+=$COMPLETION_DIR" "$HOME/.zshrc"; then
        echo "fpath+=$COMPLETION_DIR" >> "$HOME/.zshrc"
        echo -e "[${GREEN}${BOLD}SUCCESS${RESET}] Added 'fpath' configuration to ~/.zshrc."
    fi

    echo -e "[${GREEN}${BOLD}SUCCESS${RESET}] Zsh completion script installed to:"
    echo "   $COMPLETION_FILE"
    echo ""
    echo "The script will load automatically after restarting your shell or running:"
    echo "   source ~/.zshrc"
    echo ""

else
    echo -e "[${RED}${BOLD}FAIL${RESET}] Autocomplete installation is not supported for shell: $USER_SHELL"
    echo ""
fi

echo -e "${BOLD}==========================================${RESET}"
echo -e "${BOLD}Step 3: Installation Complete${RESET}"
echo -e "${BOLD}==========================================${RESET}"
echo "To run the FlavorLang interpreter, use:"
echo "   flavor --about"
echo ""
