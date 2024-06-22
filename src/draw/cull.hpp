#pragma once

#include <cstdint>
#include <glm/glm.hpp>

const uint8_t _cullIsSatMap = 1 << 0;
const uint8_t _cullIsCapture = 1 << 1;

struct Cull {
  uint8_t flags = 0;
  float x = 0;
  float y = 0;
  float z = 0;
  float fovBias = 1;
  glm::mat4 viewProjection;
};

