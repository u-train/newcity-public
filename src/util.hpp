#pragma once

#include <vector>
#include <set>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "line.hpp"
#include "item.hpp"

#include <set>
#include <csignal>

// Generate an interrupt
#define TRIP std::raise(SIGINT);

float getSqr(float val);
float distanceSqrd(glm::vec3 v1, glm::vec3 v2);
float distance2D(glm::vec3 v1, glm::vec3 v2);
float distance2DSqrd(glm::vec3 v1, glm::vec3 v2);
float distance2DSqrd(glm::vec2 v1, glm::vec2 v2);
float vecDistance(glm::vec3 v1, glm::vec3 v2);
glm::vec3 zNormal(glm::vec3 v1);
glm::vec3 uzNormal(glm::vec3 v1);
void printMatrix(glm::mat4x4 matrix);
void printVec(glm::vec3 vec);

char* formatFloat(float val);

glm::vec3 pointOfIntersection(Line l0, Line l1);
float pointLineDistance(glm::vec3 point, Line l);
float pointLineDistanceSqrd(const glm::vec3 p, const Line line);
float pointLineDistance2DSqrd(const glm::vec3 p, const Line l);
float pointRayDistanceSqrd(const glm::vec3 p, const Line l);
float lineDistance(Line l0, Line l1);
float lineDistance2D(Line l0, Line l1);
bool line2Dintersect(Line l0, Line l1);
glm::vec3 nearestPointOnLine(glm::vec3 p, Line l);
glm::vec3 lineAtZ(Line l, float z);

item iclamp(item a, item b, item c);
float lerp(float a0, float a1, float w);
glm::vec3 lerp(glm::vec3 a0, glm::vec3 a1, float w);
float nonZero(float val);
float slope(Line line);
float triangle_intersection(Line line,
  const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2);
bool boxLineIntersect(Line box, Line ml);
glm::vec3 linePlaneIntersect(Line ray, glm::vec3 p0, glm::vec3 n);
bool isInDim(glm::vec2 loc, Line dim);

bool removeFromVector(std::vector<item>* v, item n);
bool isInVector(std::vector<item>* v, item n);

bool validate(float f);
bool validate(Line l);
bool validate(glm::vec3 v);

item nextRand(item* randomSeed);
float randFloat();
float randFloat(item* randomSeed);
float randFloat(float a, float b);
float randFloat(item* randomSeed, float a, float b);
item randItem(item num);
item randItem(item* randomSeed, item num);
item randItem(item a, item b);
item randItem(item* randomSeed, item a, item b);
item randInSet(std::set<item>* s, item num);
item randRound(float val);

void free_proxy(void* ptr);

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


