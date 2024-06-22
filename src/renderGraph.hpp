#pragma once

#include "item.hpp"
#include "draw/mesh.hpp"

void renderStopLights(item ndx, int* stopLightTextureIndex);
void renderNode(item ndx);
void renderEdge(item ndx);
void renderStopDisc(Mesh* m, item ndx, glm::vec3 offset);
void renderLane(Mesh* mesh, item laneNdx, float start, float end,
    float width, item laneOffset, glm::vec3 offset, glm::vec3 tx);
void renderLane(Mesh* mesh, item laneNdx, float start, float end);
void renderLane(Mesh* mesh, item laneNdx);
void renderLaneBlock(Mesh* mesh, item ndx);
void setElementHighlight(item ndx, bool highlight);


