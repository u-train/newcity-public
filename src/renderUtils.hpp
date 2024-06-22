#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "draw/mesh.hpp"

const float signHeight = 2.5;
const float signSize = 1.5f;
const float signPoleWidth = 0.25;

const auto snowColor = glm::vec4(211/255.f, 204/255.f, 202/255.f, 1);

// Special
void mergeMeshes(Mesh* mesh, item meshNdx);

// 2D
void makeTriangle(Mesh* mesh, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 xp);
void makeTriangle(Mesh* mesh, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,
    glm::vec3 xp0, glm::vec3 xp1, glm::vec3 xp2);
void makeQuad(Mesh* mesh, glm::vec3 topRight, glm::vec3 topLeft,
  glm::vec3 bottomRight, glm::vec3 bottomLeft,
  glm::vec3 xs, glm::vec3 xe);
void makeQuad(Mesh* mesh, glm::vec3 topRight, glm::vec3 topLeft,
  glm::vec3 bottomRight, glm::vec3 bottomLeft,
  glm::vec3 trn, glm::vec3 tln, glm::vec3 brn, glm::vec3 bln,
  glm::vec3 xs, glm::vec3 xe);
void makeQuad(Mesh* mesh, glm::vec3 topRight, glm::vec3 topLeft,
  glm::vec3 bottomRight, glm::vec3 bottomLeft,
  glm::vec3 trn, glm::vec3 tln, glm::vec3 brn, glm::vec3 bln,
  glm::vec3 trx, glm::vec3 tlx, glm::vec3 brx, glm::vec3 blx);
void makeQuad(Mesh* mesh, glm::vec3 topRight, glm::vec3 topLeft,
  glm::vec3 bottomRight, glm::vec3 bottomLeft,
  glm::vec3 trx, glm::vec3 tlx, glm::vec3 brx, glm::vec3 blx);
void makeDisc(Mesh* mesh, glm::vec3 location,
    float width, int numSides, glm::vec3 xp);

// Cones, Trees, Cylinders
void makeTree(Mesh* mesh, glm::vec3 location, float height,
  float width, glm::vec3 xp, glm::vec3 trunkXp);
void makeSimpleTree(Mesh* mesh, glm::vec3 location, float height,
  float width, glm::vec3 xp);
void makeCone(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width, glm::vec3 xp, bool includeBottom);
void makeLightCone(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width);
void makeCylinder(Mesh* mesh, glm::vec3 location, float height,
  float width, int numSides, glm::vec3 xp);
void makeCylinder(Mesh* mesh, glm::vec3 location, float height,
  float width, int numSides, glm::vec3 xpb, glm::vec3 xpt);
void makeCylinder(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width, int numSides, glm::vec3 xp);
void makeCylinder(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width, int numSides, glm::vec3 xp, bool includeTop);
void makePipe(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width, float widthInner, int numSides, glm::vec3 xp);

// Cubes etc
void makeAngledCube(Mesh* mesh, glm::vec3 center, glm::vec3 dir, float height,
  glm::vec3 xp, glm::vec3 xfs, glm::vec3 xfe);
void makeCube(Mesh* mesh, glm::vec3 center, glm::vec3 size, glm::vec3 xs,
  bool includeTop, bool rounded);
void makeFlaredCube(Mesh* mesh, glm::vec3 center, glm::vec3 size, float flare,
    glm::vec3 xsb, glm::vec3 xst);
void makeFlaredCube(Mesh* mesh, glm::vec3 center, glm::vec3 size, float flare,
    glm::vec3 xsb, glm::vec3 xst, bool rounded);
void makeAngledCube(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along, glm::vec3 up,
  bool includeTop, glm::vec3 xp);
void makeIBeam(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along, glm::vec3 up,
    float innerSize, bool includeTop, bool includeBottom, glm::vec3 xs);

// Signs etc
void makeIcon(Mesh* mesh, glm::vec3 tl, glm::vec3 right, glm::vec3 down,
    glm::vec3 icon);
void makeTransitSign(Mesh* mesh, glm::vec3 loc, glm::vec3 dir, float height,
    glm::vec3 icon, glm::vec3 reverse);
void makeSign(Mesh* mesh, glm::vec3 loc, glm::vec3 dir, glm::vec3 icon, glm::vec3 reverse);
void makeDoubleSign(Mesh* mesh, glm::vec3 loc, glm::vec3 dir, glm::vec3 icon, glm::vec3 reverse);


