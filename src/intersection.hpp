#pragma once

#include <vector>
#include "item.hpp"
#include "graph.hpp"

std::vector<item> getNodeOutputs(item ndx, bool completed);
std::vector<item> getNodeInputs(item ndx, bool completed);
std::vector<item> getCompletedEdges(item ndx);
std::vector<item> getRenderableEdges(item ndx);
void setStopLightPhaseTexture(item nodeNdx);
int getOddManOut(item nodeNdx, std::vector<item> edges);
void rebuildIntersection(item nodeNdx);
void spawnVehicle(item nodeNdx, bool force);
void updateIntersections(double duration);

void swapIntersections_v(bool fromSave);
void updateIntersections_v(float duration);


