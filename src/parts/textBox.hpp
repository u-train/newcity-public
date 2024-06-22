#pragma once

#include "part.hpp"

const int _textBoxEditing = 1 << 0;

struct TextBoxState {
  char** text = 0;
  int flags = 0;
  InputCallback onSubmit;
};

Part* textBox(glm::vec2 loc, glm::vec2 size, char* text);
Part* textBox(glm::vec2 loc, glm::vec2 size, char* text, uint32_t cursorPos);
Part* textBox(glm::vec2 loc, glm::vec2 size, TextBoxState* state);
Part* textBoxLabel(glm::vec2 loc, glm::vec2 size, TextBoxState* state);
Part* textBoxLabel(glm::vec2 loc, glm::vec2 size, TextBoxState* state, glm::vec3 ico,
    const char* altText);

void focusTextBox(TextBoxState* state);


