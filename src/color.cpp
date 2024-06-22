#include <glm/vec3.hpp>
#include "color.hpp"

glm::vec3 getColorInPalette(int color) {
  if (color < 0 || color >= numColors) {
    color = 0;
  }

  int col = color / 4;
  int row = color % 4;
  return glm::vec3(col*2+1, row*2+1, 0) / paletteSize + glm::vec3(0.5,0.5,0);
}

glm::vec3 getTerrainColor(float u, bool desert) {
  if (desert) {
    return glm::vec3(u/paletteSize, 37/paletteSize,1);
  } else {
    return glm::vec3(u/paletteSize, 35/paletteSize,1);
  }
}

Line getHeatmapGradient(int hm) {
  float gradX = (35+2*hm)/paletteSize;
  return line(glm::vec3(gradX, 19/paletteSize, 0), glm::vec3(gradX, 11/paletteSize, 0));
}

