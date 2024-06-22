#pragma once

#include "part.hpp"

#include "../economy.hpp"

Part* chart(glm::vec2 start, glm::vec2 size, item econ, Statistic stat,
    item timePeriod, bool lbl, bool bar, float endDate);
Part* chart(glm::vec2 start, glm::vec2 size, item econ, Statistic stat, item timePeriod);
Part* chart(glm::vec2 start, glm::vec2 size, item econ, Statistic stat, item timePeriod,
    bool lbl, bool bar);

