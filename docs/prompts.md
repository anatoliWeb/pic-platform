[Ukrainian version](prompts.ua.md)

# Prompt Kit

The `prompts/` folder contains reusable context prompts for AI chats and local
LLM tools.

## Purpose

Use these prompts when starting a new chat about projects based on
`pic-platform`. They give the assistant the platform rules, architecture, MPLAB
integration model, and compiler constraints.

## Available Prompts

- [ChatGPT platform context](../prompts/chatgpt-platform-context.md)
- [Ollama platform context](../prompts/ollama-platform-context.md)
- [Prompt folder README](../prompts/README.md)

## Recommended Use

1. Paste the needed prompt at the start of a chat.
2. Add the concrete project request after it.
3. Keep project-specific decisions in the application layer.

## Notes

- ChatGPT prompt is more detailed.
- Ollama prompt is shorter for smaller local models.
- More prompts can be added later for code review, MPLAB integration, or module selection.
