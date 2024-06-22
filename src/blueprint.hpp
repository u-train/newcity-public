#pragma once

#include "serialize.hpp"
#include "graph.hpp"

#include "parts/textBox.hpp"

const int _blueprintExists = 1 << 0;
const int _blueprintComplete = 1 << 1;
const int _blueprintFine = 1 << 2;

const int _blueprintNodePillar = 1 << 3;

struct BlueprintNode {
  int flags;
  char zone;
  glm::vec3 loc;
  Configuration config;
};

struct BlueprintEdge {
  int flags;
  char zone[2];
  item ends[2];
  Configuration config;
};

struct Blueprint {
  int flags;
  char* name;
  money cost;
  std::vector<BlueprintEdge> edges;
  std::vector<BlueprintNode> nodes;
};

Line clampBPLine(Line l);
std::vector<item> getBPElems(Line l);
Blueprint* getDraftBlueprint();
Blueprint* getActiveBlueprint();
Blueprint* getBufferBlueprint();
std::vector<item> setBufferBlueprint(std::vector<item> graphElems, std::vector<item> lots);
void setDraftBlueprintToBuffer();
void rotateDraftBlueprint();
void flipDraftBlueprint();
item saveDraftBlueprint();
bool draftBlueprintExists();
void clearDraftBlueprint();
void clearBufferBlueprint();
item numBlueprints();
item selectedBlueprint();
Blueprint* getBlueprint(item ndx);
TextBoxState* getBlueprintTB(item ndx);
void selectBlueprint(item ndx);
void applyDraftBlueprint(glm::vec3 loc);
void applyBlueprintZones();
void queueBlueprintZones(Blueprint* bp, glm::vec3 loc);
void deleteBlueprint(item ndx);

char* blueprintToString(Blueprint* bp);
char* blueprintLibraryToString();
void readBlueprints(const char* src);

void writeBlueprints();
void readBlueprints();
void resetBlueprints();

