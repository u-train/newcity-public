#pragma once

#include "part.hpp"
#include "textBox.hpp"

#include "../blueprint.hpp"

Part* blueprintPanel(glm::vec2 start, glm::vec2 size, Blueprint* bp,
    TextBoxState* tb, item ndx);

