#pragma once

#include "../item.hpp"
#include <glm/vec3.hpp>

enum Supply {
  SuppliesRetail, SuppliesAmenity, SuppliesTourism, SuppliesFreightNeed,
  SuppliesFriend, SuppliesHome, SuppliesDorm, SuppliesHotel,
  SuppliesHomeless, SuppliesNoEduJob, SuppliesHSEduJob,
  SuppliesBclEduJob, SuppliesPhdEduJob,
  SuppliesNull,
  numSupplies
};

item routeBroker_p(item source, Supply supplies);

bool supplyTableSuggest_g(item block, Supply supply, item target);
bool supplyTableErase_g(item block, Supply supply, item target);

bool knownRouteAdd_g(item source, item dest);
void knownRouteErase_g(item source, item dest);

void broker_addLaneBlock_g(item blkNdx, glm::vec3 loc);
void broker_removeLaneBlock_g(item blkNdx);

void resetRouteBroker_g();
void initRouteBroker_g();

