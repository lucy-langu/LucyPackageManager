#!/bin/bash
GREEN="\e[32m"
CYAN="\e[36m"
RESET="\e[0m"

echo -e "${CYAN} Building LPM (Lucy Package Manager)...${RESET}"
# Setup
INSTALL_DIR="$HOME/.local/bin"
mkdir -p "$INSTALL_DIR"

# Move
mv lpm "$INSTALL_DIR/"
chmod +x "$INSTALL_DIR/lpm"

# Add to Path
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
    echo "export PATH=\"\$HOME/.local/bin:\$PATH\"" >> "$HOME/.bashrc"
    echo "export PATH=\"\$HOME/.local/bin:\$PATH\"" >> "$HOME/.zshrc"
    echo -e "${CYAN}ℹ Added $INSTALL_DIR to your PATH. Please restart your terminal or run 'source ~/.bashrc'.${RESET}"
fi

echo -e "${GREEN} LPM is successfully installed! Try running 'lpm' in your terminal.${RESET}"
