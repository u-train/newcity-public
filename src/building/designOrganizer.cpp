#include "designOrganizer.hpp"
#include "../main.hpp"

#include "building.hpp"
#include "buildingTexture.hpp"
#include "design.hpp"
#include "renderBuilding.hpp"

#include "../collisionTable.hpp"
#include "../draw/camera.hpp"
#include "../economy.hpp"
#include "../game/game.hpp"
#include "../heatmap.hpp"
#include "../land.hpp"
#include "../parts/toolbar.hpp"
#include "../parts/designConfigPanel.hpp"
#include "../platform/lua.hpp"
#include "../selection.hpp"
#include "../util.hpp"
#include "../zone.hpp"

#include "../draw/camera.hpp"
#include "../game/game.hpp"
#include "../parts/toolbar.hpp"
#include "../parts/designConfigPanel.hpp"
#include "../platform/lua.hpp"

#include "spdlog/spdlog.h"

item organizerZone = ResidentialZone;

glm::vec3 getOrganizerLocation(item dNdx) {
  Design* d = getDesign(dNdx);
  item seed = dNdx*1001;
  float positionConstant = tileSize*5 + getChunkSize()*tileSize*5;
  float positionFactor = getChunkSize() * tileSize * 10;
  float randomness = positionFactor/10-tileSize*10;
  glm::vec3 result;

  for (int i = 0; i < 100; i++) {
    float x = d->minLandValue * positionFactor + positionConstant +
      randFloat(&seed, 0, randomness);
    float y = d->minDensity * positionFactor + positionConstant +
      randFloat(&seed, 0, randomness) - d->size.y*0.5f;
    float z = c(CSuperflatHeight);
    result = glm::vec3(x,y,z);

    Box bbox = getBuildingBox(dNdx, result, glm::vec3(0,1,0));
    std::vector<item> collisions = collideBuilding(bbox, 0);

    bool anyMatches = false;
    for (int k = 0; k < collisions.size(); k++) {
      Design* other = getDesign(collisions[k]);
      if (d->zone == other->zone) {
        anyMatches = true;
        break;
      }
    }
    if (!anyMatches) break;

    randomness *= 1.02;
    //SPDLOG_INFO("getOrganizerLocation {}", d->name);
  }

  return pointOnLand(result);
}

void updateOrganizerBuilding(item dNdx) {
  Design* d = getDesign(dNdx);
  Building* b = getBuilding(dNdx);
  removeFromCollisionTable(BuildingCollisions, dNdx);
  if (d->flags & _designDisableSpawning) {
    b->flags &= ~_buildingComplete;
  } else {
    b->flags |= _buildingComplete;
  }
  b->location = getOrganizerLocation(dNdx);
  addToCollisionTable(BuildingCollisions, getBuildingBox(dNdx), dNdx);
  renderBuilding(dNdx);

  if (dNdx == getSelection()) {
    setFollowingSelection(true);
  }
}

void initOrganizerBuildings() {
  item wasSelected = getSelectedDesignNdx();

  for (int i = 1; i <= sizeBuildings(); i++) {
    removeBuilding(i);
  }
  resetBuildings();
  resetCollisionTables();
  initCollisionTables();

  for (int i = 1; i <= sizeDesigns(); i++) {
    Design* d = getDesign(i);
    glm::vec3 loc = getOrganizerLocation(i);
    item bNdx = addBuilding(loc, glm::vec3(0,1,0), i, d->zone);
    if (bNdx != i) SPDLOG_WARN("Building {} was assigned different index from design {}", bNdx, i);
    Building* b = getBuilding(bNdx);
    b->location.z = loc.z;
    bool valid = (d->flags & _designEnabled) &&
      d->zone == organizerZone;
    if (valid) {
      addToCollisionTable(BuildingCollisions, getBuildingBox(i), i);
    }
  }

  item numCompleted = 0;
  for (int i = 1; i <= sizeDesigns(); i++) {
    Design* d = getDesign(i);
    bool valid = (d->flags & _designEnabled) &&
      d->zone == organizerZone;
    bool spawnable = !(d->flags & _designDisableSpawning);

    if (!valid) {
      removeBuilding(i);
    } else if (spawnable) {
      completeBuilding(i);
      numCompleted ++;
    } else {
      addToCollisionTable(BuildingCollisions, getBuildingBox(i), i);
    }
  }

  SPDLOG_INFO("initOrganizerBuildings completed {}/{}", numCompleted,
      sizeDesigns());

  setBuildingTextureLightLevel(2);

  if (!isGameLoading()) {
    renderBuildings();
    designRender();
    renderMap_g();
    if (wasSelected != 0) {
      setSelection(SelectionBuilding, wasSelected);
      updateDesignDimensionStrings();
    }
  }
}

void startDesignOrganizer() {
  if (getGameMode() != ModeDesignOrganizer) SPDLOG_ERROR("startDesignOrganizer() but game mode is not ModeDesignOrganizer");
  SPDLOG_INFO("Starting Design Organizer");
  int landSize = 20;
  generateFlatLand(landSize);
  initHeatMaps();
  initDesigns();
  initCollisionTables();
  //initRouteBroker_g();
  initEcons();
  initOrganizerBuildings();
  resetCamera();

  /*
  int chunkSize = getChunkSize();
  float r = tileSize*(chunkSize*landSize)/2;
  int z = c(CSuperflatHeight)+1;
  item in0 = addNode(glm::vec3(tileSize*5, 0, z), config);
  item in1 = addNode(glm::vec3(tileSize*5, r*2, z), config);
  item edge = addEdge(in0, in1, config);
  complete(edge, config);

  glm::vec3 bLoc = pointOnLand(glm::vec3(tileSize*6, r+tileSize*.5f, 0));
  Line l = getLine(edge);
  glm::vec3 intersection = nearestPointOnLine(bLoc, l);
  glm::vec3 normal = bLoc - intersection;
  normal.z = 0;
  glm::vec3 unorm = normalize(normal);
  normal = unorm * c(CBuildDistance);
  bLoc = intersection + normal;

  Design* d = getDesign(1);
  glm::vec3 prevSize = d->size;
  d->size = glm::vec3(tileSize*24*2, tileSize*24*2, 0);
  completeBuilding(addBuilding(bLoc, glm::vec3(1, 0, 0), 1, d->zone));
  d->size = prevSize;
  setSpawnProbGlobal(0.1f);
  */

  initTools();
  postInitLua();
}

item getOrganizerZone() {
  return organizerZone;
}

void setOrganizerZone(item newZone) {
  organizerZone = newZone;
  if (getSelectedDesignNdx() != 0) {
    Design* d = getSelectedDesign();
    d->zone = newZone;
    pushDesignerUndoHistory();
  }
  initOrganizerBuildings();
}

