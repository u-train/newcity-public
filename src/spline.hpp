#pragma once

#include "line.hpp"
#include "serialize.hpp"

struct Spline {
  glm::vec3 end[2];
  glm::vec3 normal[2];
};

Spline spline(Line l0, Line l1);
glm::vec3 interpolateSpline(Spline s, float along);
Spline readSpline(FileBuffer* file, int version);
void writeSpline(FileBuffer* file, Spline s);

