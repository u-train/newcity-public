#pragma once

#include <glm/vec3.hpp>
#include "money.hpp"
#include "item.hpp"
#include "line.hpp"
#include "serialize.hpp"

struct Pillar {
  glm::vec3 location;
  item entity;
  item node;
  item plan;
  int flags;
};

const int _pillarExists   = 1 << 0;
const int _pillarComplete = 1 << 1;
const int _pillarSuspension = 1 << 24;

item addPillar(glm::vec3 loc, bool suspension);
void removePillar(item pillar);
item intersectPillar(Line mouseLine);
Pillar* getPillar(item ndx);
void setPillarHighlight(item ndx, bool highlight);
void completePillar(item ndx);
money pillarCost(item ndx);
const char* pillarLegalMessage(item ndx);
item sizePillars();

void writePillars(FileBuffer* file);
void readPillars(FileBuffer* file, int version);
void resetPillars();
void initPillarsEntities();
void renderPillars();


