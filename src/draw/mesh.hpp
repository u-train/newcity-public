#pragma once

#include "../item.hpp"
#include <glm/glm.hpp>
#include <GL/glew.h>

const int _meshExists = 1 << 0;
const int _meshDynDraw = 1 << 1;
const int _meshStreamDraw = 1 << 2;
const int _meshBillboard = 1 << 3;

struct RenderPoint {
  glm::vec3 point;
  glm::vec3 normal;
  glm::vec3 texture;
};

struct Triangle {
  RenderPoint points[3];
};

struct DrawPoint {
  glm::vec3 point;
  glm::vec3 normal;
  glm::ivec3 texture;
};

struct TriangleData {
  DrawPoint points[3];
};

struct Mesh {
  int flags;
  int numTriangles;
  int bufferCapacity;
  TriangleData* triangles;
};

struct BufferedMesh {
  int flags;
  int numTriangles;
  GLfloat* buffer;
  GLuint vertexVBOID;
  GLuint vaoID;
};

item addMesh();
Mesh* getMesh(item meshNdx);
void pushTriangle(Mesh* mesh, Triangle triangle);
void insertMesh(Mesh* mesh, item sourceNdx, glm::vec3 offset,
    float yaw, float pitch, glm::vec3 scal);
void reserveMesh(item meshNdx, item size);
void removeMesh(item meshNdx);
void bufferMesh(item meshNdx);
void renderMesh(item meshNdx);
BufferedMesh* getBufferedMesh(item ndx);

void vaoAdded();
void aboAdded();
void vboAdded();

void runMeshCommands();
void swapMeshCommands();
item meshesSize();
void resetMeshes();



