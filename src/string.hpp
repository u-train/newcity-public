#pragma once

#include "draw/mesh.hpp"
#include <string>

void parseFont();
void renderString(Mesh* mesh, const char* string, glm::vec3 start, float size);
void renderString(Mesh* mesh, const char* string, glm::vec3 start, float size,
    float firstLineIndent);
void renderString(Mesh* mesh, const char* string, glm::vec3 start, glm::vec3 along);
void renderString(Mesh* mesh, const char* string, glm::vec3 start, glm::vec3 along,
  glm::vec3 down);
void renderStringCentered(Mesh* mesh, const char* string,
  glm::vec3 center, glm::vec3 along, glm::vec3 down);
float characterWidth(char c);
float characterWidth(char c,char next);
float stringWidth(const char* string);
glm::vec2 stringDimensions(const char* string);
uint32_t getMaxStrLen();


