#pragma once

#include "part.hpp"

Part* block(glm::vec2 loc, glm::vec2 size);
Part* sunkenBlock(glm::vec2 loc, glm::vec2 size);
Part* darkBlock(glm::vec2 loc, glm::vec2 size);
Part* blackBlock(glm::vec2 loc, glm::vec2 size);
Part* brightBox(glm::vec2 loc, glm::vec2 size);
Part* redBlock(glm::vec2 loc, glm::vec2 size);
Part* greenBlock(glm::vec2 loc, glm::vec2 size);
Part* gradientBlock(glm::vec2 loc, glm::vec2 size, glm::vec3 grads, glm::vec3 grade);
Part* gradientBlock(glm::vec2 loc, glm::vec2 size, Line grad);
Part* colorBlock(glm::vec2 loc, glm::vec2 size, item color);


