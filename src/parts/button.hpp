#pragma once

#include "part.hpp"

Part* button(glm::vec2 start, glm::vec2 size, char* text, InputCallback callback);
Part* buttonCenter(glm::vec2 start, glm::vec2 size, char* text, InputCallback callback);
Part* button(glm::vec2 start, glm::vec3 icon, InputCallback callback);
Part* button(glm::vec2 start, glm::vec3 icon, InputCallback callback, item itemData);
Part* button(glm::vec2 start, glm::vec3 icon, glm::vec2 size, InputCallback callback,
    item itemData);
Part* button(glm::vec2 start, glm::vec3 icon, glm::vec2 size, char* text,
  InputCallback callback, item itemData);
Part* superButton(glm::vec2 start, glm::vec2 size, glm::vec3 ico, char* text,
  InputCallback callback, item itemData, InputAction action, bool highlight);


