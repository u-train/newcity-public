#include "block.hpp"

#include "../color.hpp"

Part* block(glm::vec2 loc, glm::vec2 size) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderWhiteBox;
  return result;
}

Part* darkBlock(glm::vec2 loc, glm::vec2 size) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderDarkBox;
  return result;
}

Part* blackBlock(glm::vec2 loc, glm::vec2 size) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderGradient;
  result->texture = line(colorTransparentBlack, colorTransparentBlack);
  return result;
}

Part* brightBox(glm::vec2 loc, glm::vec2 size) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderGradient;
  result->texture = line(colorRaisedGrad0, colorRaisedGrad1);
  return result;
}

Part* redBlock(glm::vec2 loc, glm::vec2 size) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderGradient;
  result->texture = line(colorRedGrad1, colorRedGrad0);
  return result;
}

Part* greenBlock(glm::vec2 loc, glm::vec2 size) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderGradient;
  result->texture = line(colorGreenGrad1, colorGreenGrad0);
  return result;
}

Part* gradientBlock(glm::vec2 loc, glm::vec2 size, Line grad) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderGradient;
  result->texture = grad;
  return result;
}

Part* gradientBlock(glm::vec2 loc, glm::vec2 size, glm::vec3 grads, glm::vec3 grade) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderGradient;
  result->texture = line(grads, grade);
  return result;
}

Part* colorBlock(glm::vec2 loc, glm::vec2 size, item colorNdx) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderGradient;
  glm::vec3 color = getColorInPalette(colorNdx);
  result->texture = line(color, color);
  return result;
}

Part* sunkenBlock(glm::vec2 loc, glm::vec2 size) {
  Part* result = part(loc);
  result->dim.end = glm::vec3(size, 0);
  result->renderMode = RenderGradient;
  result->texture = line(colorLoweredGrad1, colorLoweredGrad0);
  return result;
}

