#pragma once

#include "item.hpp"
#include "box.hpp"

#include <vector>

enum CollisionIndex {
  BuildingCollisions, LotCollisions, GraphCollisions,
  numCollisionTables
};

void resetCollisionTables();
void initCollisionTables();
void addToCollisionTable(CollisionIndex ci, Box b, item it);
void removeFromCollisionTable(CollisionIndex ci, item it);
std::vector<item> getCollisions(CollisionIndex ci, Box b, item exclude);

