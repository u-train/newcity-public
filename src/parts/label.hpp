#pragma once

#include "part.hpp"

Part* label(glm::vec2 loc, float size, char* text);
Part* label(glm::vec2 loc, glm::vec2 size, char* text);
Part* label(glm::vec2 start, float size, char* text, bool red);
Part* labelRed(glm::vec2 loc, float size, char* text);
Part* labelCenter(glm::vec2 start, glm::vec2 size, char* text);
Part* labelRight(glm::vec2 start, glm::vec2 size, char* text);
Part* multiline(glm::vec2 start, glm::vec2 size, char* text, float* ySize);


