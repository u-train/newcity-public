#pragma once

#include "shader.hpp"
#include <glm/vec4.hpp>

struct UngroupedDrawCommand {
  item mesh;
  item entity;
  item texture;
  DrawData data;
  uint8_t shader;
};

int numEntitiesDrawn();
void resetDrawBuffers();
void initDrawBuffers();
void swapDrawBuffers();

void startEntityCulling_g();
void collectEntities_g();
void collectDrawBuffer();
void runDrawCommands(Shader shader, Shader group);
void bufferABOs();
void resetVAOs(item mesh);
void writeTreeData(Cup<glm::vec4>* data, item entity);

