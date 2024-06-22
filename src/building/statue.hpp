#pragma once
#include "../item.hpp"
#include <stdint.h>

struct Statue {
  uint32_t flags;
  item meshImport;
  char *code;
  char *name;
};

void initStatueCallbacks();
item sizeStatues();
Statue *getStatue(item ndx);
item getStatueForBuilding(item buildingNdx, item decoNdx);
const char *getStatueName(item statue);
