#pragma once

#include "part.hpp"

Part* span(glm::vec2 start, float indent, glm::vec2 size, char* text, glm::vec2* end);
Part* span(glm::vec2 start, float indent, glm::vec2 size, char* text, float* y);
Part* spanCenter(glm::vec2 start, float indent, glm::vec2 size, char* text, glm::vec2* end);
Part* spanCenter(glm::vec2 start, float indent, glm::vec2 size, char* text, float* y);

