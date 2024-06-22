#pragma once

#include "part.hpp"

Part* icon(glm::vec2 start, glm::vec3 icon);
Part* icon(glm::vec2 start, glm::vec2 size, glm::vec3 icon);
Part* icon(glm::vec2 start, float ySize, float padding, glm::vec3 icon, char* text);


