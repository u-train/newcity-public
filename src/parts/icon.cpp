#include "icon.hpp"

#include "../icons.hpp"

Part* icon(glm::vec2 start, glm::vec2 size, glm::vec3 icon) {
  Part* result = part(start);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderIcon;
  result->texture = iconToSpritesheet(icon);
  return result;
}

Part* icon(glm::vec2 start, glm::vec3 ico) {
  return icon(start, glm::vec2(1,1), ico);
}

Part* icon(glm::vec2 start, float ySize, float padding, glm::vec3 icon, char* text) {
  Part* result = part(start);
  result->dim.end = glm::vec3(0, ySize, 0);
  result->padding = padding;
  result->renderMode = RenderIconAndText;
  result->texture = iconToSpritesheet(icon);
  result->text = text;
  return result;
}

