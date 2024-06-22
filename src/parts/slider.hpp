#pragma once

#include "part.hpp"

Part* slider(glm::vec2 start, glm::vec2 size, float value, InputCallback callback);
Part* slider(glm::vec2 start, glm::vec2 size, float value, InputCallback callback,
    Line tex);
Part* vslider(glm::vec2 start, glm::vec2 size, float value, InputCallback callback);


