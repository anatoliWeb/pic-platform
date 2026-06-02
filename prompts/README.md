[Ukrainian version](README.ua.md)

# Prompt Kit

This folder contains reusable prompts for using `pic-platform` in AI chats and
local LLM tools.

Use these files at the beginning of a new chat when you want the assistant to
help with projects based on this platform.

## Files

- [chatgpt-platform-context.md](chatgpt-platform-context.md) - general chat prompt for project help
- [ollama-platform-context.md](ollama-platform-context.md) - compact prompt for local Ollama models

## How to Use

1. Open the needed prompt file.
2. Paste it at the beginning of a new chat.
3. Add your project-specific request after it.

## Notes

- Prompts describe the repository architecture and usage rules.
- The platform stays external to MPLAB projects.
- Drivers and libraries should not be copied into application folders.
- Use relative paths and add required `.c` files in MPLAB.
