#pragma once

#include "item.hpp"
#include "serialize.hpp"
#include <glm/glm.hpp>

const int _cityExists = 1 << 0;
const int _cityMajor = 1 << 1;

struct City {
  int flags;
  char* name;
  item node;
  item laneBlock;
  item econ;
  glm::vec3 nodeCenter;
  glm::vec3 visualCenter;
  glm::vec3 normal;
  std::vector<item> buildings;
  item entity;
};

City* getCity(item ndx);

void resetCities();
void initCitiesEntities();
void renderCities();
void generateCities();
void updateCities(double duration);
item intersectCity(Line mouseLine);
item nearestCity(glm::vec3 loc);
void setCitiesVisible(bool viz);
item countNeighbors();

void readCities(FileBuffer* file, int version);
void writeCities(FileBuffer* file);


