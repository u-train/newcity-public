#pragma once

#include "part.hpp"

Part* amenityInfoPart(glm::vec2 loc, glm::vec2 size,
    item designNdx, item buildingNdx, bool unaffordableRed);
void amenityStats(Part* pnl, glm::vec2 loc, glm::vec2 size, item designNdx);
bool openCitipediaEffectPage(Part* part, InputEvent event);
