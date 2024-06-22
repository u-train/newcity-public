#include "panel.hpp"

Part* panel(glm::vec2 start, glm::vec2 size) {
  return panel(line(glm::vec3(start,0), glm::vec3(size,0)));
}

Part* panel(Line dim) {
  Part* result = part(dim);
  result->flags |= _partIsPanel;
  result->renderMode = RenderPanel;
  return result;
}

