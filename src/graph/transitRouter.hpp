#pragma once

#include "../item.hpp"
#include "../route/location.hpp"

#include <vector>
#include <glm/glm.hpp>

item getNumStops_r();
float transitRoutingEstimate_r(item stop1, item stop2);
float* getLineLegCostTable_r(Location legNdx);
float getLegWaitCost_r(Location legNdx);
float getStopsDistance_r(item stop1, item stop2);
std::vector<item> getNearbyStops_r(item stopNdx);
std::vector<item> getNearbyStops_r(glm::vec3 loc1);
glm::vec3 getStopLocation_r(item stopNdx);
item walkingCostStops_r(item stop1, item stop2);
std::vector<Location> getLegsForStop_r(item stopNdx);
item getLineLength_r(Location legNdx);
item getStopForLeg_r(Location legNdx);
item getStopForLeg_v(Location legNdx);
void swapTransitRouterData_g();
void swapTransitStopData_v();

