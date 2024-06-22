#pragma once

#include "part.hpp"

struct ScrollState {
  float amount = 0;
  float target = 0;
  float maxTarget = 0.0f;
  float lastCameraTime = 0;
  Part* slider = 0; // invalid before call to scrollFrame
};

void resetScrollState(ScrollState* state);
Part* scrollbox(glm::vec2 start, glm::vec2 size);
Part* scrollboxFrame(glm::vec2 start, glm::vec2 size, float innerSpace,
  ScrollState* state, Part* scrollbox);
Part* scrollboxFrame(glm::vec2 start, glm::vec2 size, ScrollState* state,
  Part* scrollbox);
Part* scrollboxFrame(glm::vec2 start, glm::vec2 size, ScrollState* state,
  Part* scrollbox, bool inverted);
Part* scrollboxFrame(glm::vec2 start, glm::vec2 size, ScrollState* state,
  Part* scrollbox, bool inverted, Line scrollbarColor);


