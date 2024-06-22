#include "query.hpp"

#include "../parts/block.hpp"
#include "../parts/slider.hpp"
#include "../parts/textBox.hpp"
#include "../parts/tooltip.hpp"
#include "../parts/article.hpp"
#include "../parts/slider.hpp"
#include "../parts/icon.hpp"
#include "../parts/button.hpp"
#include "../parts/hr.hpp"
#include "../parts/toolbar.hpp"

#include "../color.hpp"
#include "../icons.hpp"
#include "../heatmap.hpp"
#include "../label.hpp"
#include "../lot.hpp"
#include "../option.hpp"
#include "../person.hpp"
#include "../renderGraph.hpp"
#include "../renderUtils.hpp"
#include "../selection.hpp"
#include "../sound.hpp"
#include "../tutorial.hpp"
#include "../game/feature.hpp"
#include "../graph.hpp"
#include "../graph/stop.hpp"
#include "../graph/transit.hpp"
#include "../draw/camera.hpp"
#include "../draw/entity.hpp"
#include "../draw/mesh.hpp"
#include "../draw/shader.hpp"
#include "../draw/texture.hpp"

#include "tool.hpp"
#include "../main.hpp"

#include "spdlog/spdlog.h"
#include <glm/glm.hpp>

enum QuerySubTools {
  QueryInnerTool, HeatmapTool, RouteTool, LabelTool,
  numQuerySubTools
};

const Feature subToolFeature[] = {
  FQueryTool, FHeatmaps, FRouteInspector, FLabel,
};

item currentQueryTool = 0;

void setQueryInfoForce(bool reset);

static item lastHeatmap = -1;
static bool wasHeatmapSelected = false;
static bool showHeatmapInfoPanel = true;
static item hmCursor;
static item iconEntityNdx;

const HeatMapIndex heatmapOrder[] = {
  Pollution, Crime, Education, Prosperity, Value, Density,
  CommunityHM, HealthHM, 
  TrafficHeatMap, TransitHeatMap, ZoneHeatMap, RoadHeatMap,
};

const char* const heatmapLabels[] = {
  "Pollution", // X
  "Value", // V
  "Density", // F
  "Crime", // C
  "Education", // G
  "Prosperity", // J
  "Community", // N
  "Health", // H
  "Traffic", // T
  "Transit", // T
  "Zones", // Z
  "Road Map" // M
};

const char* heatmapButtonKeyLabels[] =
  {"X", "V", "F", "C", "G", "J", "N", "H", "T", "T", "Z", "M"};
const int heatmapButtonKeys[] = {
  GLFW_KEY_X,
  GLFW_KEY_V,
  GLFW_KEY_F,
  GLFW_KEY_C,
  GLFW_KEY_G,
  GLFW_KEY_J,
  GLFW_KEY_N,
  GLFW_KEY_H,
  GLFW_KEY_T,
  GLFW_KEY_T,
  GLFW_KEY_Z,
  GLFW_KEY_M,
  0,0,0,0,0,
};

const char* const heatmapMaxLabel[] = {
  "Toxic",
  "High Value",
  "City Center",
  "High Crime",
  "College Educated",
  "Prosperous",
  "Welcoming",
  "Healthy",
  "Slow Traffic",
  "Transit Line",
};

const char* const heatmapMidLabel[] = {
  "Polluted",
  "",
  "",
  "Some Crime",
  "High School",
  "",
  "",
  "",
  "",
  "",
};

const char* const heatmapMinLabel[] = {
  "Clean",
  "Low Value",
  "Rural",
  "No Crime",
  "No Education",
  "Abandoned",
  "Isolated",
  "Sick",
  "No Traffic",
  "Roadway",
};

const char* const heatmapCitipediaPage[] = {
  "heatmaps/hmPollution",
  "heatmaps/hmValue",
  "heatmaps/hmDensity",
  "heatmaps/hmCrime",
  "heatmaps/hmEducation",
  "heatmaps/hmProsperity",
  "heatmaps/hmCommunity",
  "heatmaps/hmHealth",
  "infoviews/ivTraffic",
  "infoviews/ivTransit",
  "infoviews/ivZones",
  "infoviews/ivRoadmap",
};

const char* const heatmapInfo[] = {
  "Pollution is caused by farms, factories, and traffic. "
    "Parks counteract pollution. "
    "Pollution inhibits density and land value. ",
  "Land value measures the desirability of an area. "
    "High land value areas are likely to densify. ",
  "Density measures the people and businesses in an area. ",
  "Crime is caused by economic stress such as unemployment. "
    "Crime inhibits density and land value. ",
  "Education increases land value, reduces crime, and allows factories and "
    "offices. Education is neccessary for the city to advance.",
  "Prosperity measures the economic activity of an area. "
    "Economic stress (such as unemployment) reduces prosperity and can lead "
    "to the neighborhood being abandoned.",
  "Community represents the strength of the community bonds in your city. "
  "It is increased by recreation, agriculture, social amenities, and mass "
  "transit.",
  "Health represents the health and well-being of the citizens. "
  "It is increased by recreational amenities like parks, social services, "
  "low crime, and low pollution.",
  "Transportation is the key to Prosperity. Bad traffic can prevent people "
    "from getting to jobs, shopping, and amenities.",
  "To build transit lines, select the road tool (2), then the transit tab (T)",
  "When re-zoning, existing buildings will not be destroyed, but they "
    "will be rebuilt over time.",
  "Expressways are shown in orange. Major roads are white."
   " Other roads are gray. Train lines are red."
};

bool openIssueLegend(Part* part, InputEvent event);
bool setHeatMap(Part* part, InputEvent event);
void setQueryInfoForce();

bool isQueryHeatmapSet() {
  return pinHeatmaps() && wasHeatmapSelected;
}

void heatmap_mouse_button_callback(InputEvent event) {
}

void allocHeatmapCursorEntity() {
  if (hmCursor != 0) return;

  hmCursor = addEntity(PaletteShader);
  iconEntityNdx = addEntity(WSUIShader);

  Entity* cursorEntity = getEntity(hmCursor);
  cursorEntity->texture = paletteTexture;
  setEntityVisible(hmCursor, true);
  //setEntityBringToFront(hmCursor, true);
  setEntityTransparent(hmCursor, false);
  setEntityHighlight(hmCursor, true);
  createMeshForEntity(hmCursor);

  Entity* iconEntity = getEntity(iconEntityNdx);
  iconEntity->texture = iconTexture;
  setEntityVisible(iconEntityNdx, true);
  //setEntityBringToFront(iconEntityNdx, true);
  setEntityTransparent(iconEntityNdx, false);
  setEntityHighlight(iconEntityNdx, true);
  createMeshForEntity(iconEntityNdx);

  Mesh* mesh = getMeshForEntity(hmCursor);
  makeCone(mesh, glm::vec3(0,0,1), glm::vec3(0,0,-1), 1, colorTransparentWhite, true);
  bufferMesh(cursorEntity->mesh);
}

void deallocHeatmapCursorEntity() {
  if (hmCursor == 0) return;

  removeEntity(hmCursor);
  removeEntity(iconEntityNdx);
  hmCursor = 0;
  iconEntityNdx = 0;
}

void makeIcon(Mesh* mesh, glm::vec3 ico, glm::vec3 center, float size) {
  glm::vec3 right = glm::vec3(size, 0, 0);
  glm::vec3 down = glm::vec3(0, size, 0);
  glm::vec3 topLeft = -.5f*(right+down) + center;
  Line icoLine = iconToSpritesheet(ico);

  makeQuad(mesh, topLeft, topLeft+right,
      topLeft+down, topLeft+right+down, icoLine.start, icoLine.end);
}

void heatmap_mouse_move_callback(InputEvent event) {
  if (!isHeatMapIntense() || getHeatMap() < 0) {
    deallocHeatmapCursorEntity();

  } else {
    allocHeatmapCursorEntity();

    glm::vec3 loc = landIntersect(event.mouseLine);
    if (loc.z < beachLine) loc.z = beachLine;
    float hmVal = heatMapGet(getHeatMap(), loc);
    int hmValI = round(hmVal*10);
    hmValI = glm::clamp(hmValI, 0, 10);
    float dist = getCameraDistance();
    float size = dist/15;
    glm::vec3 z = glm::vec3(0,0,size*2);
    glm::vec3 down = glm::vec3(0,size,0);
    glm::vec3 right = glm::vec3(size,0,0);
    glm::vec3 xShadowColor = iconColorDarkGray/spriteSheetSize;

    Mesh* mesh = getMeshForEntity(iconEntityNdx);
    makeQuad(mesh,
        -down*3.4f+z, -down*3.4f+right*1.2f+z, -down, -down+right*1.2f+z,
        xShadowColor, xShadowColor);

    makeIcon(mesh, iconHeatmap[getHeatMap()],
        glm::vec3(-size*.5f, -size*2.f, size*2.f)+z, size*2.f);

    for (int i = 0; i < hmValI; i ++) {
      glm::vec3 cloc = glm::vec3(i%2, i/-2, 0);
      cloc += glm::vec3(1., -1., 0);
      cloc *= size*.4f;
      cloc.y -= size;
      cloc.z = size;
      cloc += z;
      makeIcon(mesh, iconHeatmapColor[getHeatMap()], cloc, size*.4f);
    }

    bufferMesh(getEntity(iconEntityNdx)->mesh);
    placeEntity(hmCursor, loc, 0, 0, size);
    placeEntity(iconEntityNdx, loc, 0, 0);
  }
}

void heatmap_select() {
  if (wasHeatmapSelected) {
    setHeatMap((HeatMapIndex)lastHeatmap, true);
  }
  setIssuesIconsVisible();
  setQueryInfoForce();
}

void heatmap_reset() {
  if (!pinHeatmaps()) {
    setHeatMap(Pollution, false);
    setIssuesIconsVisible();
  }
  setQueryInfoForce();
  deallocHeatmapCursorEntity();
}

bool togglePinQuery(Part* part, InputEvent event) {
  setPinHeatmaps(!pinHeatmaps());
  return true;
}

bool openCitipediaHeatmapPage(Part* part, InputEvent event) {
  item hm = part->itemData;
  followLink(strdup_s(heatmapCitipediaPage[hm]));
  return true;
}

Part* heatmap_render(Line dim) {
  Part* result = panel(dim);

  r(result, label(glm::vec2(0,0), 1, strdup_s("Heatmaps")));
  //r(result, hr(glm::vec2(0,1.0), dim.end.x-toolPad*2));

  // Pin Button
  Part* pinButt = r(result, button(glm::vec2(6.67,0), iconPin, togglePinQuery));
  if (pinHeatmaps()) {
    pinButt->flags |= _partHighlight;
  }

  // Heatmaps
  for (int i = 0; i <= numHeatMaps+3; i++) {
    int hm = heatmapOrder[i];
    int hmn = hm == TrafficHeatMap ? numHeatMaps :
      hm == TransitHeatMap ? numHeatMaps+1 :
      hm == ZoneHeatMap ? numHeatMaps+2 :
      hm == RoadHeatMap ? numHeatMaps+3 :
      hm;
    if (hmn < numHeatMaps &&
        !isFeatureEnabled(featureForHeatmap(hm))) continue;
    glm::vec2 loc = i < 6 ? glm::vec2(0.f, (i)*.8f + 1) :
                       glm::vec2(5.f, (i-6)*.8f + 1);

    Part* buttContainer = panel(loc, glm::vec2(4.9f, 0.8f));
    buttContainer->renderMode = RenderTransparent;
    buttContainer->itemData = hm;
    buttContainer->inputAction = (InputAction)(ActHeatmapPollution+i);
    buttContainer->onClick = setHeatMap;
    buttContainer->flags |= _partHover;
    setPartTooltipValues(buttContainer,
      TooltipType::QuePollu+i);
    if (getHeatMap() == hm && isHeatMapIntense()) {
      buttContainer->flags |= _partHighlight;
    }
    if (blinkFeature(featureForHeatmap(hm))) {
      buttContainer->flags |= _partBlink;
    }
    r(result, buttContainer);

    if (i < 6) {
      r(buttContainer, label(glm::vec2(1.f, 0.125f), 0.7f,
            strdup_s(heatmapLabels[hmn])));
    } else {
      r(buttContainer, labelRight(glm::vec2(0.f, 0.125f), glm::vec2(3.95f, 0.7f),
            strdup_s(heatmapLabels[hmn])));
    }

    glm::vec3 hmIcon =
      hm == TrafficHeatMap ? iconCar :
      hm == TransitHeatMap ? iconBus :
      hm == ZoneHeatMap ? iconHouse :
      hm == RoadHeatMap ? iconRoad :
      iconHeatmap[hm];
    Part* ico = icon(glm::vec2(i<6?0.f:3.95f, 0.f), glm::vec2(0.85f,0.85f), hmIcon);
    KeyBind bind = getKeyBind(buttContainer->inputAction);
    if (bind.key != GLFW_KEY_UNKNOWN) {
      ico->text = strdup_s(getKeyStr(bind.key).c_str());
    }
    r(buttContainer, ico);
  }

  return result;
}

void issueLegendPanel(Part* panel, float x, float y) {
  // Let's build the Issue Icon Legend
  float initY = y;
  float textSize = 0.8f;
  float stagger = 1.0f;
  float txtYAdj = 0.1;
  float lgndPad = 0.2;

  // Header
  //r(panel, label(glm::vec2(x, y), 1.0f, strdup_s("Issue Icon Legend")));
  //r(panel, hr(glm::vec2(0, 1.1f), panel->dim.end.x - toolPad * 4));
  //y += 2.0f;

  // Icons and labels
  Part* ico = r(panel, icon(glm::vec2(x, y), iconWorker));
  ico->foregroundColor = PickerPalette::RedLight;
  r(panel, label(glm::vec2(x + stagger-lgndPad, y+txtYAdj), textSize, strdup_s("Unemployed")));
  y += textSize-0.1;

  ico = r(panel, icon(glm::vec2(x+stagger, y), iconHelpWanted));
  ico->foregroundColor = PickerPalette::RedLight;
  r(panel, label(glm::vec2(x+stagger*2-lgndPad, y+txtYAdj), textSize, strdup_s("Needs Workers")));
  y += textSize-0.1;

  ico = r(panel, icon(glm::vec2(x, y), iconFood));
  ico->foregroundColor = PickerPalette::RedLight;
  r(panel, label(glm::vec2(x + stagger-lgndPad, y+txtYAdj), textSize, strdup_s("Can't Reach Retail")));
  y = initY;
  x = x + (panel->dim.end.x - x)*.5f-.5f;

  ico = r(panel, icon(glm::vec2(x, y), iconTrade));
  ico->foregroundColor = PickerPalette::RedLight;
  r(panel, label(glm::vec2(x+stagger-lgndPad,y+txtYAdj), textSize, strdup_s("Needs Customers")));
  y += textSize-0.1;

  ico = r(panel, icon(glm::vec2(x+stagger, y), iconTruck));
  ico->foregroundColor = PickerPalette::RedLight;
  r(panel, label(glm::vec2(x + stagger*2-lgndPad, y+txtYAdj), textSize, strdup_s("Needs Freight")));
  y += textSize-0.1;

  ico = r(panel, icon(glm::vec2(x+stagger*2, y), iconSick));
  ico->foregroundColor = PickerPalette::RedLight;
  r(panel, label(glm::vec2(x + stagger*3-lgndPad, y+txtYAdj), textSize, strdup_s("Sick")));
}

void routeInspectorInstructionPanel(Part* result);
void queryInstructionPanel(Part* panel) {
  if (isQueryTool()) {
    if (currentQueryTool == QueryInnerTool) {
      r(panel, label(glm::vec2(1, 2), .85, strdup_s(
        "Left click on any object to\n"
        "learn about it.")));
      return;

    } else if (currentQueryTool == HeatmapTool &&
        !isHeatMapIntense()) {
      r(panel, label(glm::vec2(1, 2), .85, strdup_s(
        "Select a Heatmap to\n"
        "observe the state of\n"
        "your city.\n")));
      return;

    } else if (currentQueryTool == RouteTool) {
      routeInspectorInstructionPanel(panel);
      return;

    } else if (currentQueryTool == LabelTool) {
      r(panel, label(glm::vec2(1, 2), .85, strdup_s(
        "Left click anywhere to\n"
        "place a label.\n"
        "Then type text and\n"
        "press enter to finish.\n")));
      return;
    }
  }

  if (isHeatMapIntense()) {
    int hm = getHeatMap();
    int hmn = hm == TrafficHeatMap ? numHeatMaps :
      hm == TransitHeatMap ? numHeatMaps+1 :
      hm == ZoneHeatMap ? numHeatMaps+2 :
      hm == RoadHeatMap ? numHeatMaps+3 :
      hm;

    float sliderX = 2;
    float sliderLabelSizeX = 5;
    float multilineWidth = 10;
    float multilineTextSize = 0.8f;

    if (hmn == Prosperity) {
      sliderX = 1;
      sliderLabelSizeX = 3;
      multilineWidth = 9;
      multilineTextSize = 0.75f;
    }

    if (hmn <= numHeatMaps) {
      Line heatmapGrad = getHeatmapGradient(hm);
      r(panel, gradientBlock(glm::vec2(sliderX,.9), glm::vec2(1,4.9),
        heatmapGrad.end, heatmapGrad.start));
      r(panel, labelCenter(glm::vec2(0,0), glm::vec2(sliderLabelSizeX, .85),
        strdup_s(heatmapMaxLabel[hmn])));
      r(panel, labelCenter(glm::vec2(0,5.9), glm::vec2(sliderLabelSizeX, .85),
        strdup_s(heatmapMinLabel[hmn])));
      if (hmn == Education) {
        r(panel, labelCenter(glm::vec2(0,3), glm::vec2(sliderX, .85),
          strdup_s("HS")));
      }
    }

    float y = 0;
    Part* info = multiline(glm::vec2(3.5,0),
        glm::vec2(multilineWidth,multilineTextSize),
        strdup_s(heatmapInfo[hmn]), &y);
    if (hmn != Prosperity) {
      info->dim.start.y = 3.5 - y*.5;
    }
    r(panel, info);

    if (hmn == Prosperity) {
      issueLegendPanel(panel, sliderX+1.5, 3.5);
    }

    r(panel, button(glm::vec2(8.75f, 5.9f), iconCitipedia, glm::vec2(4.75f, 0.85f), strdup_s("Learn More"), openCitipediaHeatmapPage, hmn));
  }
}

bool isShowHeatmapInfoPanel() {
  return showHeatmapInfoPanel;
}

bool setHeatMap(Part* part, InputEvent event) {
  if (part == NULL) {
    SPDLOG_ERROR("Null part in setHeatMap");
    return false;
  }

  HeatMapIndex ndx = (HeatMapIndex)part->itemData;
  int hmn = ndx == TrafficHeatMap ? numHeatMaps :
    ndx == TransitHeatMap ? numHeatMaps+1 :
    ndx == ZoneHeatMap ? numHeatMaps+2 :
    ndx == RoadHeatMap ? numHeatMaps+3 :
    ndx;

  if (getHeatMap() == ndx && isHeatMapIntense()) {
    setHeatMap(Pollution, false);
    wasHeatmapSelected = false;
  } else {
    setHeatMap(ndx, true);
    wasHeatmapSelected = true;
    lastHeatmap = ndx;

    // If no tool is open and Query is enabled, 
    // open the Query tool to the Heatmap subtool
    if(getCurrentTool() == 0 && isFeatureEnabled(FQueryTool) && !pinHeatmaps()) {
      setTool(1);
      setQuerySubTool(QuerySubTools::HeatmapTool);
    }
  }
  setQueryInfoForce();
  stopBlinkingFeature(featureForHeatmap(ndx));
  return true;
}

void toggleQueryInfoPanel() {
  if (isHeatMapIntense()) {
    showHeatmapInfoPanel = !showHeatmapInfoPanel;

  } else {
    setShowInstructions(!getShowInstructions());
  }

  setQueryInfoForce();
}

bool heatmap_visible() {
  return true;
}

Tool toolHeatmap = {
  heatmap_mouse_button_callback,
  heatmap_mouse_move_callback,
  heatmap_select,
  heatmap_reset,
  heatmap_visible,
  iconGrid,
  heatmap_render,
  queryInstructionPanel,
};


SelectionTypes querySelectionType = NoSelection;
item querySelection = 0;
item vehicleSelection = 0;
float vehicleSelectionTime = 0;
static bool isVehicleFollowMode = false;
static bool isQueryMode = true;

void setQueryHighlight(bool highlight) {
  if (querySelectionType != getSelectionType() ||
    querySelection != getSelection()
  ) {
    setHighlight(highlight, querySelectionType, querySelection);
  }
}

void query_inner_mouse_button_callback(InputEvent event) {
  if (!isQueryMode) return;

  int button = event.button;
  int action = event.action;

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    playSound(_soundClickCancel);
    setSelection(querySelectionType, querySelection);
  }
}

void query_inner_mouse_move_callback(InputEvent event) {
  setQueryHighlight(false);

  if (isVehicleFollowMode) {
    if (getSelection() == 0) {
      //setSelection(SelectionVehicle, getRandomVehicle());
      if (sizePeople() > 0) {
        setSelection(SelectionPerson, randItem(sizePeople())+1);
      }

    } else if (!isFollowingSelection()) {
      isVehicleFollowMode = false;

    } else if (getSelectionType() == SelectionVehicle) {
      Vehicle* vehicle = getVehicle(getSelection());
      float angle = pi_o*3/2 - vehicle->yaw;
      if (angle < 0) {
        angle += pi_o*2;
      }
      setHorizontalCameraAngle(angle);
    }
    return;
  }

  if (!isQueryMode) return;

  Line mouseLine = event.mouseLine;
  float minDist = tileSize;
  querySelection = 0;
  querySelectionType = NoSelection;

  item vehicleNdx = nearestVehicle(mouseLine);
  if (vehicleNdx > 0) {
    Vehicle* vehicle = getVehicle(vehicleNdx);
    // Selecting vehicles is preferred
    float dist = pointLineDistance(vehicle->location, mouseLine);
    if (dist < tileSize*.5f) {
      if (vehicleSelection != vehicleNdx) {
        vehicleSelection = vehicleNdx;
        vehicleSelectionTime = getCameraTime();
      }

      if (vehicleSelectionTime < getCameraTime() - 0.1) {
        minDist = dist*.25f;
        querySelection = vehicleSelection;
        querySelectionType = SelectionVehicle;
      }
    }
  }

  item buildingNdx = nearestBuilding(mouseLine);
  if (buildingNdx > 0) {
    glm::vec3 landPoint = landIntersect(mouseLine);
    float dist = boxDistance(getBuildingBox(buildingNdx), landPoint);
    if (dist < minDist) {
      minDist = dist;
      querySelection = buildingNdx;
      querySelectionType = SelectionBuilding;
    }
  }

  item stopNdx = nearestStop(mouseLine);
  if (stopNdx > 0) {
    Stop* s = getStop(stopNdx);
    float dist = pointLineDistance(s->location, mouseLine) *.25f;
    if (dist < minDist) {
      querySelection = stopNdx;
      querySelectionType = SelectionStop;
      minDist = dist;
    }
  }

  item graphElemNdx = nearestElement(mouseLine, true);
  if (graphElemNdx != 0) {
    float dist = lineDistance(getLine(graphElemNdx), mouseLine);
    if (dist < minDist) {
      minDist = dist;
      querySelection = graphElemNdx;
      querySelectionType = SelectionGraphElement;
    }
  }

  if (areLotsVisible()) {
    item lotNdx = nearestLot(mouseLine);
    if (lotNdx > 0) {
      Lot* lot = getLot(lotNdx);
      float dist = pointLineDistance(lot->loc, mouseLine);
      if (dist < minDist) {
        minDist = dist;
        querySelection = lotNdx;
        querySelectionType = SelectionLot;
      }
    }
  }

  setQueryHighlight(true);
}

void query_inner_select() {
}

void query_inner_reset() {
  isVehicleFollowMode = false;
  setQueryHighlight(false);
  querySelectionType = NoSelection;
  querySelection = 0;
}

bool startVehicleFollowMode(Part* part, InputEvent event) {
  setQueryHighlight(false);
  //setHeatMap(Pollution, false);
  //setQueryInfoForce();
  querySelectionType = NoSelection;
  querySelection = 0;
  clearSelection();
  //setMenuMode(HideUI);
  isVehicleFollowMode = true;
  return true;
}

bool stopVehicleFollowMode(Part* part, InputEvent event) {
  isVehicleFollowMode = false;
  setFollowingSelection(false);
  return true;
}

void stopVehicleFollowMode() {
  isVehicleFollowMode = false;
}

Part* query_inner_render(Line dim) {
  Part* result = panel(dim);

  r(result, label(glm::vec2(0,0), 1, strdup_s("Query")));
  //r(result, hr(glm::vec2(0,1.0), dim.end.x-toolPad*2));

  /*
  // Toggle Query Mode
  Part* queryButtContainer = panel(glm::vec2(0.f, 1.f), glm::vec2(3.33f, 1.f));
  Part* queryIco = button(glm::vec2(0.f, 0.f), iconQuery, toggleQueryTool);
  queryIco->text = strdup_s("Q");
  queryButtContainer->renderMode = RenderTransparent;
  queryButtContainer->onKeyDown = toggleQueryTool;
  queryButtContainer->onClick = toggleQueryTool;
  queryButtContainer->flags |= _partHover;
  if (isQueryMode) {
    queryButtContainer->flags |= _partHighlight;
  }
  r(queryButtContainer, label(glm::vec2(1.f, 0.125f), 0.75, strdup_s("Query")));
  r(queryButtContainer, queryIco);
  r(result, queryButtContainer);
  */

  // Demo Mode
  Part* carButtContainer = panel(glm::vec2(3.34f, 2.5f), glm::vec2(3.33f, 1.f));
  Part* carIco = icon(glm::vec2(0.f, 0.f), iconFamily);
  carButtContainer->renderMode = RenderTransparent;
  carButtContainer->onClick = startVehicleFollowMode;
  carButtContainer->flags |= _partHover;
  setPartTooltipValues(carButtContainer,
    TooltipType::QueDemo);
  if (isVehicleFollowMode) {
    carButtContainer->flags |= _partHighlight;
  }
  r(carButtContainer, label(glm::vec2(1.f, 0.125f), 0.75f,
        strdup_s("Random")));
  r(carButtContainer, carIco);
  r(result, carButtContainer);

  return result;
}

bool query_inner_visible() {
  return true;
}

Tool toolQueryInner = {
  query_inner_mouse_button_callback,
  query_inner_mouse_move_callback,
  query_inner_select,
  query_inner_reset,
  query_inner_visible,
  iconQuery,
  query_inner_render,
  queryInstructionPanel,
};



static item currentLabel = 0;
static item lastHoverLabel = 0;
static item lastLabelFlags = _labelExists | (6 << _labelSizeShift);
TextBoxState labelTbState;

void correctCameraDistance() {
  //float fontSize = getLabelFontSize(currentLabel);
  Label* label = getLabel(currentLabel);
  if (label->entity == 0) return;
  Entity* e = getEntity(label->entity);
  float minDist = e->simpleDistance*2.f;
  float maxDist = e->maxCameraDistance*.02f;
  float dist = getCameraDistance();
  if (dist < minDist) {
    setCameraDistance(minDist);
  } else if (dist > maxDist) {
    setCameraDistance(maxDist);
  }
}

void label_mouse_button_callback(InputEvent event) {
  int button = event.button;
  int action = event.action;

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    playSound(_soundClickCancel);

    labelTbState.flags |= _textBoxEditing;
    currentLabel = intersectLabel(event.mouseLine);
    if (currentLabel == 0) {
      currentLabel = addLabel(landIntersect(event.mouseLine),
          lastLabelFlags);
    }

    Label* lab = getLabel(currentLabel);
    setCameraTarget(lab->location);
    correctCameraDistance();
  }
}

void label_mouse_move_callback(InputEvent event) {
  if (lastHoverLabel != 0) {
    setLabelHighlight(lastHoverLabel, true);
    lastHoverLabel = 0;
  }

  lastHoverLabel = intersectLabel(event.mouseLine);
  setLabelHighlight(lastHoverLabel, false);
}

void label_select() {
  setLabelsVisible(true);
}

void label_reset() {
  currentLabel = 0;
  labelTbState.flags &= ~_textBoxEditing;

  if (lastHoverLabel != 0) {
    setLabelHighlight(lastHoverLabel, true);
    lastHoverLabel = 0;
  }
}

bool finishEditingLabel(Part* part, InputEvent event) {
  SPDLOG_INFO("finishEditingLabel");
  labelTbState.flags &= ~_textBoxEditing;
  currentLabel = 0;
  focusTextBox(0);
  return true;
}

bool adjustLabelSize(Part* part, InputEvent event) {
  int size = round(part->vecData.x * 15);
  Label* lab = getLabel(currentLabel);
  lab->flags &= ~_labelSizeMask;
  lab->flags |= size << _labelSizeShift;
  correctCameraDistance();
  return true;
}

bool setLabelRed(Part* part, InputEvent event) {
  Label* lab = getLabel(currentLabel);
  lab->flags |= _labelRed;
  lab->flags &= ~_labelBlue;
  return true;
}

bool setLabelBlue(Part* part, InputEvent event) {
  Label* lab = getLabel(currentLabel);
  lab->flags |= _labelBlue;
  lab->flags &= ~_labelRed;
  return true;
}

bool setLabelWhite(Part* part, InputEvent event) {
  Label* lab = getLabel(currentLabel);
  lab->flags &= ~_labelBlue;
  lab->flags &= ~_labelRed;
  return true;
}

bool removeLabel(Part* part, InputEvent event) {
  removeLabel(currentLabel);
  labelTbState.flags &= ~_textBoxEditing;
  currentLabel = 0;
  focusTextBox(0);
  return true;
}

Part* label_render(Line dim) {
  Part* result = panel(dim);

  r(result, label(glm::vec2(0,0), 1, strdup_s("Label")));
  //r(result, hr(glm::vec2(0,1.0), dim.end.x-toolPad*2));

  if (currentLabel == 0) {
    r(result, icon(glm::vec2(4.f,2.5f), glm::vec2(1,1), iconPointer));

  } else {
    if (labelTbState.flags & _textBoxEditing) {

      renderLabel(currentLabel);
      Label* lab = getLabel(currentLabel);
      lastLabelFlags = lab->flags;
      labelTbState.flags |= _textBoxEditing;
      labelTbState.text = &(lab->text);
      focusTextBox(&labelTbState);

      r(result, label(glm::vec2(0,0), 1, strdup_s("Label")));

      Part* tb = r(result, textBoxLabel(glm::vec2(0.5f, 2.f), glm::vec2(9.f, 1.f),
          &labelTbState));
      tb->onCustom = finishEditingLabel;

      r(result, button(glm::vec2(.5f, 4.f), iconNo, removeLabel));
      r(result, button(glm::vec2(2.f, 4.f), iconRed, setLabelRed));
      //r(result, button(glm::vec2(2.5f, 5.f), iconZoneColor[RetailZone],
            //setLabelBlue));
      r(result, button(glm::vec2(3.f, 4.f), iconWhite, setLabelWhite));

      float val = (lab->flags & _labelSizeMask) >> _labelSizeShift;
      val /= 15;
      r(result, slider(glm::vec2(5.f, 4.f), glm::vec2(4.5f, 1.f), val, adjustLabelSize));

      return result;

    } else {
      Label* lab = getLabel(currentLabel);
      if (strlen(lab->text) == 0) {
        removeLabel(currentLabel);
      }
      currentLabel = 0;
    }
  }

  return result;
}

bool label_visible() {
  return true;
}

Tool toolLabel = {
  label_mouse_button_callback,
  label_mouse_move_callback,
  label_select,
  label_reset,
  label_visible,
  iconMessage,
  label_render,
  queryInstructionPanel,
};

#include "../color.hpp"
#include "../economy.hpp"
#include "../route/router.hpp"
#include "../time.hpp"

static item cursorEntityNdx[2];
static glm::vec3 cursorLoc[2];
static GraphLocation cursorGraphLoc[2];
static bool cursorSet[2] = {false,false};
static int atEnd = 0;
item routeEntity;
Route route[2];

void allocRouteCursorEntity() {
  if (cursorEntityNdx[0] != 0) return;

  for (int i = 0; i < 2; i++) {

    cursorEntityNdx[i] = addEntity(PaletteShader);
    Entity* cursorEntity = getEntity(cursorEntityNdx[i]);
    cursorEntity->texture = paletteTexture;
    setEntityVisible(cursorEntityNdx[i], false);
    //setEntityBringToFront(cursorEntityNdx[i], true);
    setEntityTransparent(cursorEntityNdx[i], false);
    setEntityHighlight(cursorEntityNdx[i], true);
    createMeshForEntity(cursorEntityNdx[i]);

    Mesh* mesh = getMeshForEntity(cursorEntityNdx[i]);
    makeCone(mesh, glm::vec3(0,0,1), glm::vec3(0,0,-1),
        1, i ? colorBlue : colorRed, true);
    bufferMesh(cursorEntity->mesh);
  }

  routeEntity = addEntity(PaletteShader);
  Entity* routeEnt = getEntity(routeEntity);
  routeEnt->texture = paletteTexture;
  setEntityVisible(routeEntity, false);
  //setEntityBringToFront(routeEntity, true);
  setEntityTransparent(routeEntity, false);
  setEntityHighlight(routeEntity, true);
  setEntityRaise(routeEntity, 3);
  setCull(routeEntity, 1000*1000*1000, 1000*1000*1000);
  createMeshForEntity(routeEntity);
}

void deallocRouteCursorEntity() {
  if (cursorEntityNdx[0] == 0) return;

  removeEntity(cursorEntityNdx[0]);
  removeEntity(cursorEntityNdx[1]);
  removeEntity(routeEntity);
  cursorEntityNdx[0] = 0;
  cursorEntityNdx[1] = 0;
  routeEntity = 0;
}

void routes_select() {
}

void routes_reset() {
  deallocRouteCursorEntity();
  cursorLoc[0] = glm::vec3(0,0,0);
  cursorLoc[1] = glm::vec3(0,0,0);
  cursorGraphLoc[0] = {0,0};
  cursorGraphLoc[1] = {0,0};
  cursorSet[0] = false;
  cursorSet[1] = false;
  atEnd = 0;
  route[0].steps.clear();
  route[1].steps.clear();
}

void renderCursor() {
  allocRouteCursorEntity();

  float dist = getCameraDistance();
  float size = dist/15;

  for (int i = 0; i < 2; i++) {
    setEntityVisible(cursorEntityNdx[i], cursorSet[i]);
    if (cursorSet[i]) {
      Entity* cursorEntity = getEntity(cursorEntityNdx[i]);
      placeEntity(cursorEntityNdx[i], cursorLoc[i], 0, 0, size);
    }
  }

  setEntityVisible(routeEntity, cursorSet[0] && cursorSet[1]);
  if (cursorSet[0] && cursorSet[1] && (
        route[0].source/10 != cursorGraphLoc[0].lane/10 ||
        route[0].destination/10 != cursorGraphLoc[1].lane/10)) {

    glm::vec3 offset = (cursorLoc[0] + cursorLoc[1]) * .5f;
    glm::vec3 offsetDown = offset + glm::vec3(0,0,-4);
    placeEntity(routeEntity, offset+glm::vec3(0,0,100), 0, 0);

    item meshNdx = getEntity(routeEntity)->mesh;
    Mesh* mesh = getMesh(meshNdx);

    for (int j = 0; j < 2; j++) {
      route[j].source = getLaneBlockIndex(cursorGraphLoc[0].lane);
      route[j].destination = getLaneBlockIndex(cursorGraphLoc[1].lane);
      std::vector<Location> steps = routeInstant(route[j].source,
          route[j].destination, j);

      if (j == 1 && route[j].steps.size() > 0) {
        steps.insert(steps.begin(), route[j].source);
        steps.push_back(route[j].destination);
      }

      route[j].steps.fromVector(steps);

      int stepS = steps.size();
      item lastStepType = -1;
      Location lastTransitLoc = 0;

      for (int i = 0; i < stepS; i++) {
        Location step = steps[i];
        Location nextStep = i+1 < stepS ? steps[i+1] : 0;
        item stepType = locationType(step);
        item nextStepType = locationType(nextStep);

        if (stepType == LocLaneBlock) {
          if (step < 10) {
            // no-op, possible error

          } else {
            item lane = step;
            float start = 0;
            float end = -1;
            if (nextStepType == LocDap) {
              //lane = getLaneBlockIndex(step) + getBlockLanes(step)-1;
              if (lastStepType == LocLaneBlock) {
                start = 0;
                end = locationNdx(nextStep)-5;

              } else {
                start = locationNdx(nextStep)+5;
                end = -1;
              }
            }

            renderLane(mesh, lane, start, end,
                c(CTransitLineWidth)*.7f, 0, offset, colorRed);
          }

        } else if (stepType == LocDap) {
          // no-op

        } else if (stepType == LocTransitLeg) {
          item stopNdx = getStopForLeg_g(step);
          renderStopDisc(mesh, stopNdx, offsetDown);

          if (lastTransitLoc > 0) {
            item lineNdx = locationLineNdx(lastTransitLoc);
            if (lineNdx == locationLineNdx(step)) {
              item endNdx = locationLegNdx(step);
              for (int l = locationLegNdx(lastTransitLoc); l < endNdx; l++) {
                renderTransitLeg(meshNdx, lineNdx, l, offsetDown);
              }
            }
            lastTransitLoc = step;
          } else {
            lastTransitLoc = step;
          }
        }

        lastStepType = stepType;
      }
    }

    bufferMesh(meshNdx);
  }
}

void routes_mouse_button_callback(InputEvent event) {
  int button = event.button;
  int action = event.action;

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    if (cursorSet[0]) {
      playSound(_soundClickCancel);
      atEnd = atEnd == 0 ? 1 : 2;
      reportTutorialUpdate(InspectedRoute);
    }

  } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    playSound(_soundClickPrimary);
    atEnd = atEnd == 2 ? 1 : 0;
    cursorSet[1] = false;
  }
}

void routes_mouse_move_callback(InputEvent event) {
  if (atEnd > 1) {
    renderCursor();
    return;
  }

  GraphLocation loc = graphLocation(event.mouseLine);
  if (loc.lane < 10) {
    cursorSet[atEnd] = false;

  } else {
    cursorSet[atEnd] = true;
    Lane* lane = getLane(loc.lane);
    LaneBlock* blk = getLaneBlock(loc.lane);
    loc.dap = glm::clamp(loc.dap, c(CTileSize), lane->length - c(CTileSize));
    loc.dap = round(loc.dap/c(CTileSize)) * c(CTileSize);
    cursorGraphLoc[atEnd] = loc;
    cursorLoc[atEnd] = getLocation(loc);
  }

  renderCursor();
}

bool routes_visible() {
  return true;
}

Part* routes_render(Line dim) {
  Part* result = panel(dim);
  float y = 0;
  r(result, label(glm::vec2(0,0), 1, strdup_s("Route Inspector")));
  //r(result, hr(glm::vec2(0,1.0), dim.end.x-toolPad*2));
  r(result, multiline(glm::vec2(0, 1.25), glm::vec2(10, .8), strdup_s(
    "Left click on a road to see routes"
    " starting there."
    " Left click on a different"
    " road to inspect that route."), &y));

  if (c(CValueOfTime) != 0) {
    float dollarPerHour = c(CValueOfTime)*getInflation();
    float daysPerDollar = 1.f/(dollarPerHour*24);
    char* moneyVal = printMoneyString(dollarPerHour);
    char* timeVal = printDurationString(daysPerDollar);
    Part* p = r(result, multiline(glm::vec2(0,4), glm::vec2(10, 0.75),
          sprintf_o("Note: citizens value their time"
            " at %s per hour, so they'll wait %s to save one dollar.",
            moneyVal, timeVal), &y));
    free(moneyVal);
    free(timeVal);
  }

  return result;
}

void routeInspectorInstructionPanel(Part* result) {
  Line dim = result->dim;

  int routesValid = 0;
  int betterRoute = -1;
  float betterRouteCost = -1;
  float scl = 0.7;
  float routeCost[2] = {0,0};
  float columnSpacing = 9;

  //r(result, hr(glm::vec2(0, 1), dim.end.x-result->padding*2));

  if (cursorSet[0] && cursorSet[1]) {
    for (int i = 0; i < 2; i++) {
      Cup<Location>* steps = &route[i].steps;
      int stepS = steps->size();
      RouteInfo info = computeRouteInfo_g(steps, i, false);

      if (stepS == 0) {
        float x = columnSpacing*i;
        float y = 7-scl-result->padding*1;
        r(result, icon(glm::vec2(x,y), glm::vec2(scl, scl),  iconNo));
        r(result, label(glm::vec2(x+1,y), scl, strdup_s("No Route")));
      }

      float y = 0;
      float x = columnSpacing*i;
      float xs = 2;

      r(result, labelRight(glm::vec2(x,y), glm::vec2(4,1.f),
            strdup_s(i ? "By Transit" : "By Car")));
      y += 1;

      if (stepS == 0) continue;

      r(result, labelRight(glm::vec2(x+2,y), glm::vec2(xs,scl),
            printDurationString(info.travelTime)));
      y += scl;

      if (info.walkingTime > 0) {
        r(result, labelRight(glm::vec2(x,y), glm::vec2(2,scl), strdup_s("+ ")));
        r(result, labelRight(glm::vec2(x+2,y), glm::vec2(xs,scl),
              printDurationString(info.walkingTime)));
      }
      y += scl;

      if (info.waitTime > 0) {
        r(result, labelRight(glm::vec2(x,y), glm::vec2(2,scl), strdup_s("+ ")));
        r(result, labelRight(glm::vec2(x+2,y), glm::vec2(xs,scl),
              printDurationString(info.waitTime)));
      }
      y += scl;

      r(result, hr(glm::vec2(x, y), 4));
      y += 0.1;
      if (info.multiplier != 1) {
        r(result, labelRight(glm::vec2(x,y), glm::vec2(2,scl), sprintf_o("%1.1f x ",
                info.multiplier)));
        Part* bonusCont = r(result, panel(glm::vec2(x-.5f,y), glm::vec2(2,2)));
        bonusCont->renderMode = RenderTransparent;
        r(bonusCont, icon(glm::vec2(.95,.6),
              glm::vec2(.5f, .5f),  iconUp));
        r(bonusCont, labelCenter(glm::vec2(0,0.8), glm::vec2(2.3,0.6),
              strdup_s(info.multiplier < 1 ? "Bonus!" : "Penalty!")));
      }
      r(result, labelRight(glm::vec2(x+2,y), glm::vec2(xs,scl),
            printDurationString(info.time)));
      y += scl;

      if (!i) {
        r(result, labelRight(glm::vec2(x,y), glm::vec2(2,scl), strdup_s("+ ")));
        r(result, labelRight(glm::vec2(x+2,y), glm::vec2(xs,scl),
              printMoneyString(info.fuelMaintCost)));
      }
      y += scl;

      if (i && info.ticketCost > 0) {
        r(result, labelRight(glm::vec2(x,y), glm::vec2(2,scl), strdup_s("+ ")));
        r(result, labelRight(glm::vec2(x+2,y), glm::vec2(xs,scl),
              printMoneyString(info.ticketCost)));
      }
      y += scl;

      r(result, hr(glm::vec2(x, y), 4));
      y += 0.1;
      r(result, labelRight(glm::vec2(x,y), glm::vec2(2,scl), strdup_s("= ")));
      r(result, labelRight(glm::vec2(x+2,y), glm::vec2(xs,scl),
            printDurationString(info.costAdjustedTime)));
      y += scl;

      routesValid ++;
      if (betterRoute == -1 || info.costAdjustedTime < betterRouteCost) {
        betterRouteCost = info.costAdjustedTime;
        betterRoute = i;
      }
    }
  }

  if (betterRoute >= 0 && routesValid == 2) {
    float x = columnSpacing*betterRoute;
    float y = 7-scl-result->padding*1;
    r(result, icon(glm::vec2(x,y), glm::vec2(scl, scl),  iconYes));
    r(result, label(glm::vec2(x+1,y), scl, strdup_s("Preferred")));
  }

  float y = 1;
  float x = 4.75;
  r(result, label(glm::vec2(x,y), scl, strdup_s("Travel Time")));
  y += scl;
  r(result, label(glm::vec2(x,y), scl, strdup_s("Walking Time")));
  y += scl;
  r(result, label(glm::vec2(x,y), scl, strdup_s("Wait Time")));
  y += scl;
  r(result, hr(glm::vec2(x, y), 3.5));
  y += 0.1;
  r(result, label(glm::vec2(x,y), scl, strdup_s("Total Time")));
  y += scl;
  r(result, label(glm::vec2(x,y), scl, strdup_s("Fuel & Maint.")));
  y += scl;
  r(result, label(glm::vec2(x,y), scl, strdup_s("Ticket Price")));
  y += scl;
  r(result, hr(glm::vec2(x, y), 3.5));
  y += 0.1;
  r(result, label(glm::vec2(x,y), scl, strdup_s("Cost-Adjusted\nTime")));

  //r(result, block(glm::vec2(4.75, 0), glm::vec2(0.05, dim.end.y-result->padding*2)));
  //r(result, block(glm::vec2(9.75, 0), glm::vec2(0.05, dim.end.y-result->padding*2)));
}

Tool toolRoute = {
  routes_mouse_button_callback,
  routes_mouse_move_callback,
  routes_select,
  routes_reset,
  routes_visible,
  iconRoute,
  routes_render,
  queryInstructionPanel,
};


Tool* querySubTools[numQuerySubTools] = {
  &toolQueryInner,
  &toolHeatmap,
  &toolRoute,
  &toolLabel,
};

void query_mouse_button_callback(InputEvent event) {
  querySubTools[currentQueryTool]->mouse_button_callback(event);
}

void query_mouse_move_callback(InputEvent event) {
  querySubTools[currentQueryTool]->mouse_move_callback(event);
}

void query_select() {
  querySubTools[currentQueryTool]->select();
  setQueryInfoForce(false);
}

void query_reset() {
  querySubTools[currentQueryTool]->reset();
  setQueryInfoForce(true);
}

void setQuerySubTool(item tool) {
  querySubTools[currentQueryTool]->reset();
  currentQueryTool = tool;
  stopBlinkingFeature(subToolFeature[tool]);
  querySubTools[currentQueryTool]->select();
  setQueryInfoForce(false);
  if (currentQueryTool == RouteTool) {
    reportTutorialUpdate(SelectedRouteInspector);
  }
}

bool toggleQueryTool(Part* part, InputEvent event) {
  setQuerySubTool(part->itemData);
  return true;
}

Part* query_render(Line dim) {
  Part* result = panel(dim);

  Line innerDim = line(glm::vec3(0,0,0), dim.end);
  innerDim.end.x -= 0.2;
  innerDim.end.y -= 1.2;
  r(result, querySubTools[currentQueryTool]->render(innerDim));

  Part* tabPanel = panel(glm::vec2(0,6), glm::vec2(10,1));
  tabPanel->flags |= _partLowered;
  r(result, tabPanel);

  for (int i = 0; i < numQuerySubTools; i++) {
    if (!isFeatureEnabled(subToolFeature[i])) continue;
    Part* butt = button(glm::vec2(i, 0.f), querySubTools[i]->icon,
        toggleQueryTool, i);
    butt->inputAction = (InputAction)(((int)ActQuerySubtool+i));

    bool blink = blinkFeature(subToolFeature[i]);
    if (!blink && i == HeatmapTool) {
      for (int j = 0; j < numHeatMaps; j++) {
        if (blinkFeature(featureForHeatmap(j))) blink = true;
      }
    }

    if (blink) {
      butt->flags |= _partBlink;
    }

    if (i == currentQueryTool) {
      butt->flags |= _partHighlight;
    }
    r(tabPanel, butt);
  }

  return result;
}

std::string query_name() {
  return "Query";
}

bool query_visible() {
  return true;
  //#ifdef LP_DEBUG
    //return true;
  //#else
    //return getGameMode() == ModeTest || numPeople() > 1000;
  //#endif
}

Tool toolQuery = {
  query_mouse_button_callback,
  query_mouse_move_callback,
  query_select,
  query_reset,
  query_visible,
  iconQuery,
  query_render,
  queryInstructionPanel,
  query_name,
};

void setQueryInfoForce(bool reset) {
  if (isHeatMapIntense() && showHeatmapInfoPanel) {
    toolQuery.flags |= _toolForceInstructions;
  } else if (!reset && currentQueryTool == RouteTool) {
    toolQuery.flags |= _toolForceInstructions;
  } else {
    toolQuery.flags &= ~_toolForceInstructions;
  }
}

bool isRouteTool() {
  return isQueryTool() && currentQueryTool == RouteTool;
}

void setQueryInfoForce() {
  setQueryInfoForce(false);
}

Tool* getToolQuery() {
  return &toolQuery;
}

