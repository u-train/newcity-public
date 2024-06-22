#include "icons.hpp"

#include "item.hpp"

#include "spdlog/spdlog.h"

#include <string>
#include <unordered_map>
using namespace std;

std::unordered_map<std::string, item> iconsMap;
const float spriteSpacing = 1.0 / (spriteSheetSize * spriteResolution);

const glm::vec3 iconsTable[numIcons] = {
  #define ICO(N,V) V,
  #include "iconsEnum.hpp"
  #undef ICO
};

glm::vec3 getIcon(const char* ico) {
  return iconsTable[iconsMap[ico]];
}

void assignIcon(item ico, const char* name) {
  iconsMap[name] = ico;
}

void initIconsMap() {
  iconsMap.clear();
  #define ICO(N,V) assignIcon(Icons::Icon##N, "Icon" #N);
  #include "iconsEnum.hpp"
  #undef ICO
}

Line iconToSpritesheet(glm::vec3 icon, float wind) {
  return line(
    glm::vec3(icon.x/spriteSheetSize + spriteSpacing,
      icon.y/spriteSheetSize + spriteSpacing, wind),
    glm::vec3((icon.x+1)/spriteSheetSize - spriteSpacing,
      (icon.y+1)/spriteSheetSize - spriteSpacing, wind));
}

Line iconToSpritesheet(glm::vec3 icon) {
  return iconToSpritesheet(icon, 0);
}

