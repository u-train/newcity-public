#pragma once
#include <glm/vec3.hpp>

struct Line {
  glm::vec3 start, end;
};

Line line(glm::vec3 start, glm::vec3 end);
float length(Line line);
