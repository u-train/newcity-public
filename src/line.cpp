#include "line.hpp"
#include "util.hpp"

Line line(glm::vec3 start, glm::vec3 end) {
  Line result;
  result.start = start;
  result.end = end;
  return result;
}

float length(Line line) {
  return vecDistance(line.start, line.end);
}
