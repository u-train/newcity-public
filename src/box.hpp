#pragma once

#include "line.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

struct Box {
  glm::vec2 corner;
  glm::vec2 axis0;
  glm::vec2 axis1;
  Line lines[4];
};

Box box(glm::vec2 corner, glm::vec2 axis0, glm::vec2 axis1);
Box box(Line l, float width);
Box alignedBox(Line l);
Box box(glm::vec2 center, float radius);
Box growBox(Box box, float growth);
std::vector<glm::vec2> boxCorners(Box b);
bool boxIntersect(Box b0, Box b1);
std::vector<glm::vec3> boxLineIntersect(const Box b, Line l);
float boxDistance(Box b, glm::vec3 p);
float boxSizeSqrd(Box b);

