#include "hr.hpp"

static const float ruleWidth = 0.1;

Part* hr(glm::vec2 loc, float length) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(length, ruleWidth, 0);
  result->renderMode = RenderWhiteBox;
  return result;
}

