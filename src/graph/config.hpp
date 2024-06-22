#pragma once

#include "../item.hpp"
#include <glm/glm.hpp>
#include <stdint.h>
#include <vector>

enum FeatureType {
  UntypedFeature,
  TractionFeature,
  PowerFeature,
  AutomationFeature,
  numFeatureTypes
};

struct GraphFeature {
  uint8_t type;
  char *code;
  char *name;
  char *text;
  glm::vec3 icon;
  uint8_t maxCars;
};

item getGraphFeature(const char *code);
GraphFeature *getGraphFeature(item ndx);
void initGraphConfigCallbacks();
void resetGraphConfig();
std::vector<item> getGraphFeatures(item type);
