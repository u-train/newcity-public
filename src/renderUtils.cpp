#include "renderUtils.hpp"
#include "draw/mesh.hpp"
#include "icons.hpp"
#include "util.hpp"
#include "game/constants.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

static glm::vec3 up = glm::vec3(0, 0, 1);

void makeTriangle(Mesh* mesh, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 xp) {
  glm::vec3 n0 = cross(p0-p1, p0-p2);
  Triangle t1 = {{
    {p0, n0, xp},
    {p1, n0, xp},
    {p2, n0, xp},
  }};
  pushTriangle(mesh, t1);
}

void makeTriangle(Mesh* mesh, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2,
    glm::vec3 xp0, glm::vec3 xp1, glm::vec3 xp2) {
  glm::vec3 n0 = cross(p0-p1, p0-p2);
  Triangle t1 = {{
    {p0, n0, xp0},
    {p1, n0, xp1},
    {p2, n0, xp2},
  }};
  pushTriangle(mesh, t1);
}

void makeQuad(Mesh* mesh, glm::vec3 topRight, glm::vec3 topLeft,
  glm::vec3 bottomRight, glm::vec3 bottomLeft,
  glm::vec3 xs, glm::vec3 xe) {

  glm::vec3 n0 = cross(bottomRight - topRight, bottomRight - topLeft);
  glm::vec3 n1 = cross(topLeft - bottomLeft, topLeft - bottomRight);

  Triangle triangle1 = {{
    {bottomRight, n0, glm::vec3(xs.x, xe.y, xe.z)},
    {topRight,    n0, xs},
    {topLeft,     n0, glm::vec3(xe.x, xs.y, xs.z)},
  }};
  pushTriangle(mesh, triangle1);

  Triangle triangle2 = {{
    {topLeft,     n1, glm::vec3(xe.x, xs.y, xs.z)},
    {bottomLeft,  n1, xe},
    {bottomRight, n1, glm::vec3(xs.x, xe.y, xe.z)},
  }};
  pushTriangle(mesh, triangle2);
}

void makeQuad(Mesh* mesh, glm::vec3 topRight, glm::vec3 topLeft,
  glm::vec3 bottomRight, glm::vec3 bottomLeft,
  glm::vec3 trn, glm::vec3 tln, glm::vec3 brn, glm::vec3 bln,
  glm::vec3 xs, glm::vec3 xe) {

  Triangle triangle1 = {{
    {bottomRight, brn, glm::vec3(xs.x, xe.y, xe.z)},
    {topRight,    trn, xs},
    {topLeft,     tln, glm::vec3(xe.x, xs.y, xs.z)},
  }};
  pushTriangle(mesh, triangle1);

  Triangle triangle2 = {{
    {topLeft,     tln, glm::vec3(xe.x, xs.y, xs.z)},
    {bottomLeft,  bln, xe},
    {bottomRight, brn, glm::vec3(xs.x, xe.y, xe.z)},
  }};
  pushTriangle(mesh, triangle2);
}

void makeQuad(Mesh* mesh, glm::vec3 topRight, glm::vec3 topLeft,
  glm::vec3 bottomRight, glm::vec3 bottomLeft,
  glm::vec3 trx, glm::vec3 tlx, glm::vec3 brx, glm::vec3 blx) {

  glm::vec3 n0 = cross(bottomRight - topRight, bottomRight - topLeft);
  glm::vec3 n1 = cross(topLeft - bottomLeft, topLeft - bottomRight);

  Triangle triangle1 = {{
    {bottomRight, n0, brx},
    {topRight,    n0, trx},
    {topLeft,     n0, tlx},
  }};
  pushTriangle(mesh, triangle1);

  Triangle triangle2 = {{
    {topLeft,     n1, tlx},
    {bottomLeft,  n1, blx},
    {bottomRight, n1, brx},
  }};
  pushTriangle(mesh, triangle2);
}

void makeQuad(Mesh* mesh, glm::vec3 topRight, glm::vec3 topLeft,
  glm::vec3 bottomRight, glm::vec3 bottomLeft,
  glm::vec3 trn, glm::vec3 tln, glm::vec3 brn, glm::vec3 bln,
  glm::vec3 trx, glm::vec3 tlx, glm::vec3 brx, glm::vec3 blx) {

  Triangle triangle1 = {{
    {bottomRight, brn, brx},
    {topRight,    trn, trx},
    {topLeft,     tln, tlx},
  }};
  pushTriangle(mesh, triangle1);

  Triangle triangle2 = {{
    {topLeft,     tln, tlx},
    {bottomLeft,  bln, blx},
    {bottomRight, brn, brx},
  }};
  pushTriangle(mesh, triangle2);
}

void makeDisc(Mesh* mesh, glm::vec3 location,
    float width, int numSides, glm::vec3 xp) {

  glm::vec3 axis = glm::vec3(0,0,1);
  glm::vec3 normal = normalize(cross(axis, glm::vec3(0, 1.f, 0.f))) * (width/2);
  glm::vec3 *points1 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  for (int i = 0; i < numSides; i ++) {
    float theta = pi_o * 2 * i / numSides;
    glm::vec3 dir = rotate(normal, theta, axis);
    points1[i] = dir + location;
  }

  for (int i = 0; i < numSides; i ++) {
    glm::vec3 p0 = points1[(i + 1) % numSides];
    glm::vec3 p1 = points1[i];

    Triangle t3 = {{
      {p1, up, xp},
      {p0, up, xp},
      {location, up, xp},
    }};
    pushTriangle(mesh, t3);
  }
}

void makeTree(Mesh* mesh, glm::vec3 location, float height, float width,
    glm::vec3 xp, glm::vec3 trunkXp) {

  width *= .5f;
  const float trunkHeight = 5;
  const float segmentFrac = 0.25;
  trunkXp.z = 0;
  makeCylinder(mesh, location+glm::vec3(0,0,-trunkHeight),
      trunkHeight, width*.25f, 12, trunkXp);
  glm::vec3 top = location + glm::vec3(0,0,height);
  glm::vec3 halfHeight = location + glm::vec3(0,0,height*segmentFrac);
  xp.z = 0;
  glm::vec3 xpt = glm::vec3(glm::vec2(xp), height);
  glm::vec3 xpHalf = glm::vec3(glm::vec2(xp), height*.25);
  const int numSides = 12;
  glm::vec3 points0[numSides];
  glm::vec3 points1[numSides];
  for (int i = 0; i < numSides; i ++) {
    float theta = pi_o * 2 * i / numSides;
    glm::vec3 dir = glm::vec3(cos(theta)*width, sin(theta)*width, 0);
    points0[i] = dir + location;
    points1[i] = dir*(1-segmentFrac) + halfHeight;
  }

  for (int i = 0; i < numSides; i ++) {
    glm::vec3 p0 = points1[(i + 1) % numSides];
    glm::vec3 p1 = points1[i];
    glm::vec3 p2 = top;
    glm::vec3 p3 = points0[(i + 1) % numSides];
    glm::vec3 p4 = points0[i];
    glm::vec3 n0 = normalize(p0 - location);
    glm::vec3 n1 = normalize(p1 - location);
    glm::vec3 n3 = normalize(p3 - location);
    glm::vec3 n4 = normalize(p4 - location);

    Triangle t1 = {{
      {p2, glm::vec3(0,0,1), xpt},
      {p1, n1, xpHalf},
      {p0, n0, xpHalf},
    }};
    pushTriangle(mesh, t1);

    makeQuad(mesh, p0, p1, p3, p4,
      n0, n1, n3, n4, xpHalf, xpHalf, xp, xp);
  }
}

void makeSimpleTree(Mesh* mesh, glm::vec3 location, float height,
    float width, glm::vec3 xp) {

  width *= .5f;
  glm::vec3 top = location + glm::vec3(0,0,height);
  xp.z = 0;
  glm::vec3 xpt = glm::vec3(glm::vec2(xp), height);
  const int numSides = 6;
  glm::vec3 points0[numSides];
  for (int i = 0; i < numSides; i ++) {
    float theta = pi_o * 2 * i / numSides;
    glm::vec3 dir = glm::vec3(cos(theta)*width, sin(theta)*width, 0);
    points0[i] = dir + location;
  }

  for (int i = 0; i < numSides; i ++) {
    glm::vec3 p0 = points0[(i + 1) % numSides];
    glm::vec3 p1 = points0[i];
    glm::vec3 p2 = top;
    glm::vec3 n0 = normalize(p0 - location);
    glm::vec3 n1 = normalize(p1 - location);

    Triangle t1 = {{
      {p2, glm::vec3(0,0,1), xpt},
      {p1, n1, xp},
      {p0, n0, xp},
    }};
    pushTriangle(mesh, t1);
  }
}

void makeCone(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width, glm::vec3 xp, bool includeBottom) {

  glm::vec3 normal = normalize(cross(axis, glm::vec3(0, 1.f, 1.f))) * (width/2);
  glm::vec3 top = location + axis;
  const int numSides = 24;
  glm::vec3 points0[numSides];
  for (int i = 0; i < numSides; i ++) {
    float theta = pi_o * 2 * i / numSides;
    glm::vec3 dir = rotate(normal, theta, axis);
    points0[i] = dir + location;
  }

  for (int i = 0; i < numSides; i ++) {
    glm::vec3 p0 = points0[(i + 1) % numSides];
    glm::vec3 p1 = points0[i];
    glm::vec3 p2 = top;
    glm::vec3 n0 = normalize(p0 - location);
    glm::vec3 n1 = normalize(p1 - location);

    Triangle t1 = {{
      {p2, normalize(n0+n1), xp},
      {p1, n1, xp},
      {p0, n0, xp},
    }};
    pushTriangle(mesh, t1);

    if (includeBottom) {
      Triangle t3 = {{
        {p0, -axis, xp},
        {p1, -axis, xp},
        {location, -axis, xp},
      }};
      pushTriangle(mesh, t3);
    }
  }
}

void makeLightCone(Mesh* mesh, glm::vec3 location, glm::vec3 axis, float width) {
  glm::vec3 xp = glm::vec3(0,0,0);
  glm::vec3 normal = normalize(cross(axis, glm::vec3(0, 1.f, 1.f))) * (width/2);
  const int numSides = 5;
  glm::vec3 points0[numSides];
  for (int i = 0; i < numSides; i ++) {
    float theta = pi_o * 2 * i / numSides;
    glm::vec3 dir = rotate(normal, theta, axis);
    dir.z /= 2;
    points0[i] = location + axis + dir;
  }

  for (int i = 0; i < numSides; i ++) {
    glm::vec3 p0 = points0[(i + 1) % numSides];
    glm::vec3 p1 = points0[i];

    Triangle t1 = {{
      {location, location, xp},
      {p0, location, xp},
      {p1, location, xp},
    }};
    pushTriangle(mesh, t1);
  }
}

void makeCylinder(Mesh* mesh, glm::vec3 location, float height,
  float width, int numSides, glm::vec3 xp) {
  makeCylinder(mesh, location, height, width, numSides, xp, xp);
}

void makeCylinder(Mesh* mesh, glm::vec3 location, float height,
  float width, int numSides, glm::vec3 xpb, glm::vec3 xpt) {

  glm::vec3 top = location + glm::vec3(0,0,height);
  glm::vec3 *points0 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  glm::vec3 *points1 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  for (int i = 0; i < numSides; i ++) {
    float theta = pi_o * 2 * i / numSides;
    glm::vec3 dir = glm::vec3(cos(theta)*width, sin(theta)*width, 0);
    points0[i] = dir + location;
    points1[i] = dir + top;
  }

  for (int i = 0; i < numSides; i ++) {
    glm::vec3 p0 = points0[(i + 1) % numSides];
    glm::vec3 p1 = points0[i];
    glm::vec3 p2 = points1[(i + 1) % numSides];
    glm::vec3 p3 = points1[i];

    Triangle t1 = {{
      {p2, p2-top, xpt},
      {p1, p1-location, xpb},
      {p0, p0-location, xpb},
    }};
    pushTriangle(mesh, t1);

    Triangle t2 = {{
      {p2, p2-top, xpt},
      {p3, p3-top, xpt},
      {p1, p1-location, xpb},
    }};
    pushTriangle(mesh, t2);

    Triangle t3 = {{
      {p3, up, xpt},
      {p2, up, xpt},
      {top, up, xpt},
    }};
    pushTriangle(mesh, t3);
  }
}

void makeCylinder(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width, int numSides, glm::vec3 xp) {
  makeCylinder(mesh, location, axis, width, numSides, xp, true);
}

void makeCylinder(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width, int numSides, glm::vec3 xp, bool includeTop) {

  glm::vec3 normal = normalize(cross(axis, glm::vec3(0, 1.f, 1.f))) * (width/2);
  glm::vec3 top = location + axis;
  glm::vec3 *points0 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  glm::vec3 *points1 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  for (int i = 0; i < numSides; i ++) {
    float theta = pi_o * 2 * i / numSides;
    glm::vec3 dir = rotate(normal, theta, axis);
    points0[i] = dir + location;
    points1[i] = dir + top;
  }

  for (int i = 0; i < numSides; i ++) {
    glm::vec3 p0 = points0[(i + 1) % numSides];
    glm::vec3 p1 = points0[i];
    glm::vec3 p2 = points1[(i + 1) % numSides];
    glm::vec3 p3 = points1[i];

    Triangle t1 = {{
      {p2, p2-top, xp},
      {p1, p1-location, xp},
      {p0, p0-location, xp},
    }};
    pushTriangle(mesh, t1);

    Triangle t2 = {{
      {p2, p2-top, xp},
      {p3, p3-top, xp},
      {p1, p1-location, xp},
    }};
    pushTriangle(mesh, t2);

    if (includeTop) {
      Triangle t3 = {{
        {p3, up, xp},
        {p2, up, xp},
        {top, up, xp},
      }};
      pushTriangle(mesh, t3);
    }
  }
}

void makePipe(Mesh* mesh, glm::vec3 location, glm::vec3 axis,
  float width, float widthInner, int numSides, glm::vec3 xp) {

  glm::vec3 normal = normalize(cross(axis, glm::vec3(0, 1.f, 1.f))) * (width/2);
  glm::vec3 normalInner = normalize(cross(axis, glm::vec3(0, 1.f, 1.f))) * (widthInner/2);
  glm::vec3 top = location + axis;
  glm::vec3 *points0 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  glm::vec3 *points1 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  glm::vec3 *points2 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  glm::vec3 *points3 = (glm::vec3*)alloca(sizeof(glm::vec3)*numSides);
  for (int i = 0; i < numSides; i ++) {
    float theta = pi_o * 2 * i / numSides;
    glm::vec3 dir = rotate(normal, theta, axis);
    glm::vec3 dirInner = rotate(normalInner, theta, axis);
    points0[i] = dir + location;
    points1[i] = dir + top;
    points2[i] = dirInner + location;
    points3[i] = dirInner + top;
  }

  for (int i = 0; i < numSides; i ++) {
    glm::vec3 p0 = points0[(i + 1) % numSides];
    glm::vec3 p1 = points0[i];
    glm::vec3 p2 = points1[(i + 1) % numSides];
    glm::vec3 p3 = points1[i];
    glm::vec3 p4 = points2[(i + 1) % numSides];
    glm::vec3 p5 = points2[i];
    glm::vec3 p6 = points3[(i + 1) % numSides];
    glm::vec3 p7 = points3[i];
    glm::vec3 n0 = p2-top;
    glm::vec3 n1 = p3-top;

    makeQuad(mesh, p1, p0, p3, p2,
      n1, n0, n1, n0, xp, xp);
    makeQuad(mesh, p3, p2, p7, p6, xp, xp);
    makeQuad(mesh, p4, p5, p6, p7,
      -n0, -n1, -n0, -n1, xp, xp);
  }
}

void makeFlaredCube(Mesh* mesh, glm::vec3 center, glm::vec3 size, float flare,
    glm::vec3 xsb, glm::vec3 xst, bool rounded) {
  size = size * 0.5f;
  center.z += size.z;
  glm::vec3 tlf = center + glm::vec3( size.x, -size.y,  size.z);
  glm::vec3 tlb = center + glm::vec3( size.x,  size.y,  size.z);
  glm::vec3 trf = center + glm::vec3(-size.x, -size.y,  size.z);
  glm::vec3 trb = center + glm::vec3(-size.x,  size.y,  size.z);
  size += glm::vec3(flare, flare, 0);
  glm::vec3 blf = center + glm::vec3( size.x, -size.y, -size.z);
  glm::vec3 blb = center + glm::vec3( size.x,  size.y, -size.z);
  glm::vec3 brf = center + glm::vec3(-size.x, -size.y, -size.z);
  glm::vec3 brb = center + glm::vec3(-size.x,  size.y, -size.z);

  //glm::vec3 ntlb = glm::vec3( 1,  1,  1);
  //glm::vec3 ntlf = glm::vec3( 1, -1,  1);
  //glm::vec3 ntrb = glm::vec3(-1,  1,  1);
  //glm::vec3 ntrf = glm::vec3(-1, -1,  1);
  if (rounded) {
    glm::vec3 ntlb = glm::vec3( 1,  1, 1);
    glm::vec3 ntlf = glm::vec3( 1, -1, 1);
    glm::vec3 ntrb = glm::vec3(-1,  1, 1);
    glm::vec3 ntrf = glm::vec3(-1, -1, 1);
    glm::vec3 nblb = glm::vec3( 1,  1, 0);
    glm::vec3 nblf = glm::vec3( 1, -1, 0);
    glm::vec3 nbrb = glm::vec3(-1,  1, 0);
    glm::vec3 nbrf = glm::vec3(-1, -1, 0);

    makeQuad(mesh, tlf, trf, blf, brf, ntlf, ntrf, nblf, nbrf, xst, xsb);
    makeQuad(mesh, tlb, tlf, blb, blf, ntlb, ntlf, nblb, nblf, xst, xsb);
    makeQuad(mesh, tlb, blb, trb, brb, ntlb, nblb, ntrb, nbrb, xst, xsb);
    makeQuad(mesh, brb, brf, trb, trf, nbrb, nbrf, ntrb, ntrf, xsb, xst);
    //top
    makeQuad(mesh, trf, tlf, trb, tlb, ntrf, ntlf, ntrb, ntlb, xst, xst);

  } else {
    makeQuad(mesh, tlf, trf, blf, brf, xst, xsb);
    makeQuad(mesh, tlb, tlf, blb, blf, xst, xsb);
    makeQuad(mesh, trb, tlb, brb, blb, xst, xsb);
    makeQuad(mesh, brb, brf, trb, trf, xsb, xst);
    //top
    makeQuad(mesh, trf, tlf, trb, tlb, xst, xst);
  }
}

void makeFlaredCube(Mesh* mesh, glm::vec3 center, glm::vec3 size, float flare,
    glm::vec3 xsb, glm::vec3 xst) {
  makeFlaredCube(mesh, center, size, flare, xsb, xst, false);
}

void makeCube(Mesh* mesh, glm::vec3 center, glm::vec3 size, glm::vec3 xs,
  bool includeTop, bool rounded) {

  size = size * 0.5f;
  center.z += size.z;
  glm::vec3 tlf = center + glm::vec3( size.x, -size.y,  size.z);
  glm::vec3 tlb = center + glm::vec3( size.x,  size.y,  size.z);
  glm::vec3 trf = center + glm::vec3(-size.x, -size.y,  size.z);
  glm::vec3 trb = center + glm::vec3(-size.x,  size.y,  size.z);
  glm::vec3 blf = center + glm::vec3( size.x, -size.y, -size.z);
  glm::vec3 blb = center + glm::vec3( size.x,  size.y, -size.z);
  glm::vec3 brf = center + glm::vec3(-size.x, -size.y, -size.z);
  glm::vec3 brb = center + glm::vec3(-size.x,  size.y, -size.z);

  if (rounded) {
    glm::vec3 ntlb = glm::vec3( 1,  1,  0);
    glm::vec3 ntlf = glm::vec3( 1, -1,  0);
    glm::vec3 ntrb = glm::vec3(-1,  1,  0);
    glm::vec3 ntrf = glm::vec3(-1, -1,  0);
    glm::vec3 nblb = glm::vec3( 1,  1, -1);
    glm::vec3 nblf = glm::vec3( 1, -1, -1);
    glm::vec3 nbrb = glm::vec3(-1,  1, -1);
    glm::vec3 nbrf = glm::vec3(-1, -1, -1);

    makeQuad(mesh, tlf, trf, blf, brf,
      ntlf, ntrf, nblf, nbrf, xs, xs);
    makeQuad(mesh, tlb, tlf, blb, blf,
      ntlb, ntlf, nblb, nblf, xs, xs);
    makeQuad(mesh, tlb, blb, trb, brb,
      ntlb, nblb, ntrb, nbrb, xs, xs);
    makeQuad(mesh, brb, brf, trb, trf,
      nbrb, nbrf, ntrb, ntrf, xs, xs);
    if (includeTop) {
      makeQuad(mesh, trf, tlf, trb, tlb,
        ntrf, ntlf, ntrb, ntlb, xs, xs);
    }

  } else {
    makeQuad(mesh, tlf, trf, blf, brf, xs, xs);
    makeQuad(mesh, tlb, tlf, blb, blf, xs, xs);
    makeQuad(mesh, tlb, blb, trb, brb, xs, xs);
    makeQuad(mesh, brb, brf, trb, trf, xs, xs);
    if (includeTop) {
      makeQuad(mesh, trf, tlf, trb, tlb, xs, xs);
    }
  }
  /*
  makeQuad(mesh, brf, blf, trf, tlf);
  makeQuad(mesh, trf, tlf, trb, tlb);
  makeQuad(mesh, tlb, tlf, blb, blf);

  makeQuad(mesh, brb, trb, blb, tlb);
  makeQuad(mesh, brb, blb, brf, blf);
  makeQuad(mesh, brb, brf, trb, trf);
  */
}

void makeAngledCube(Mesh* mesh, glm::vec3 center, glm::vec3 dir, float height,
  glm::vec3 xp, glm::vec3 xfs, glm::vec3 xfe) {

  height/=2;
  dir/=2;
  glm::vec3 norm = zNormal(dir);
  glm::vec3 up = glm::vec3(0, 0, height);
  center.z += height;
  glm::vec3 tlf = center + norm - dir + up;
  glm::vec3 tlb = center + norm + dir + up;
  glm::vec3 trf = center - norm - dir + up;
  glm::vec3 trb = center - norm + dir + up;
  glm::vec3 blf = center + norm - dir - up;
  glm::vec3 blb = center + norm + dir - up;
  glm::vec3 brf = center - norm - dir - up;
  glm::vec3 brb = center - norm + dir - up;

  makeQuad(mesh, tlf, trf, blf, brf, xp, xp);
  makeQuad(mesh, tlb, tlf, blb, blf, xp, xp);
  makeQuad(mesh, trb, tlb, brb, blb, xfs, xfe);
  makeQuad(mesh, brb, brf, trb, trf, xp, xp);
  makeQuad(mesh, trf, tlf, trb, tlb, xp, xp);
}


void makeAngledCube(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along, glm::vec3 up,
  bool includeTop, glm::vec3 xp) {

  glm::vec3 blf = loc                     ;
  glm::vec3 blb = loc         + along     ;
  glm::vec3 brf = loc + right             ;
  glm::vec3 brb = loc + right + along     ;
  glm::vec3 tlf = loc                 + up;
  glm::vec3 tlb = loc         + along + up;
  glm::vec3 trf = loc + right         + up;
  glm::vec3 trb = loc + right + along + up;

  makeQuad(mesh, tlf, trf, blf, brf, xp, xp);
  makeQuad(mesh, trb, tlb, brb, blb, xp, xp);
  makeQuad(mesh, tlb, tlf, blb, blf, xp, xp);
  makeQuad(mesh, brb, brf, trb, trf, xp, xp);
  if (includeTop) {
    makeQuad(mesh, trf, tlf, trb, tlb, xp, xp);
  }
}

/*
void makeCubeSides(Mesh* mesh, glm::vec3 center, glm::vec3 size) {
  size = size * 0.5f;
  center.z += size.z;
  glm::vec3 tlf = center + glm::vec3(size.x, size.y, size.z);
  glm::vec3 tlb = center + glm::vec3(size.x, size.y, -size.z);
  glm::vec3 trf = center + glm::vec3(-size.x, size.y, size.z);
  glm::vec3 trb = center + glm::vec3(-size.x, size.y, -size.z);
  glm::vec3 blf = center + glm::vec3(size.x, -size.y, size.z);
  glm::vec3 blb = center + glm::vec3(size.x, -size.y, -size.z);
  glm::vec3 brf = center + glm::vec3(-size.x, -size.y, size.z);
  glm::vec3 brb = center + glm::vec3(-size.x, -size.y, -size.z);

  makeQuad(mesh, trf, tlf, trb, tlb);
  makeQuad(mesh, tlb, tlf, blb, blf);

  makeQuad(mesh, brb, blb, brf, blf);
  makeQuad(mesh, brb, brf, trb, trf);
}
*/

void makeIcon(Mesh* mesh, glm::vec3 tl, glm::vec3 right, glm::vec3 down,
    glm::vec3 icon) {
  Line iconL = iconToSpritesheet(icon, 0.f);
  makeQuad(mesh, tl, tl+right, tl+down, tl+right+down,
      iconL.start, iconL.end);
}

void makeTransitSign(Mesh* mesh, glm::vec3 loc, glm::vec3 dir, float height,
    glm::vec3 icon, glm::vec3 reverse) {
  glm::vec3 xpp = glm::vec3(0.5/spriteSheetSize, 4.5/spriteSheetSize, 1);
  glm::vec3 right = normalize(glm::vec3(dir.y, -dir.x, 0))*signSize;
  glm::vec3 down = -up*signSize;
  glm::vec3 tl = loc + up*height*2.f - right*.25f +
    normalize(dir)*signPoleWidth/2.f;
  Line iconL = iconToSpritesheet(icon, 1.f);
  Line reverseL = iconToSpritesheet(reverse, 1.f);

  makeCylinder(mesh, loc+down, up*height*2.f-down,
    signPoleWidth, 6, xpp);
  makeQuad(mesh, tl, tl+right, tl+down, tl+right+down,
    iconL.start, iconL.end);
  makeQuad(mesh, tl+right, tl, tl+right+down, tl+down,
    reverseL.start, reverseL.end);
}

void makeSign(Mesh* mesh, glm::vec3 loc, glm::vec3 dir, glm::vec3 icon, glm::vec3 reverse) {
  glm::vec3 xpp = glm::vec3(0.5/spriteSheetSize, 4.5/spriteSheetSize, 1);
  glm::vec3 right = normalize(glm::vec3(dir.y, -dir.x, 0))*signSize;
  glm::vec3 down = -up*signSize;
  glm::vec3 tl = loc + up*signHeight - right/2.f + normalize(dir)*signPoleWidth/2.f;
  Line iconL = iconToSpritesheet(icon, 1.f);
  Line reverseL = iconToSpritesheet(reverse, 1.f);

  makeCylinder(mesh, loc+down, up*signHeight-down,
    signPoleWidth, 6, xpp);
  makeQuad(mesh, tl, tl+right, tl+down, tl+right+down,
    iconL.start, iconL.end);
  makeQuad(mesh, tl+right, tl, tl+right+down, tl+down,
    reverseL.start, reverseL.end);
}

void makeDoubleSign(Mesh* mesh, glm::vec3 loc, glm::vec3 dir,
  glm::vec3 icon, glm::vec3 reverse
) {
  glm::vec3 xpp = glm::vec3(0.5/spriteSheetSize, 4.5/spriteSheetSize, 1.);
  glm::vec3 right = normalize(glm::vec3(dir.y, -dir.x, 0))*signSize*2.f;
  glm::vec3 down = -up*signSize*2.f;
  dir = normalize(dir);
  glm::vec3 tl = loc + up*signHeight*2.f - right/2.f + dir*signPoleWidth/2.f;
  glm::vec3 dtl = tl + right - dir*signPoleWidth;
  glm::vec3 ll = loc-right/4.f;
  glm::vec3 lr = loc+right/4.f;
  Line iconL = iconToSpritesheet(icon, 1.f);
  Line reverseL = iconToSpritesheet(reverse, 1.f);

  makeCylinder(mesh, lr-up*signHeight, up*signHeight*3.f,
    signPoleWidth, 6, xpp);
  makeCylinder(mesh, ll-up*signHeight, up*signHeight*3.f,
    signPoleWidth, 6, xpp);

  makeQuad(mesh, tl, tl+right, tl+down, tl+right+down,
    iconL.start, iconL.end);
  makeQuad(mesh, tl+right, tl, tl+right+down, tl+down,
    reverseL.start, reverseL.end);

  makeQuad(mesh, dtl, dtl-right, dtl+down, dtl-right+down,
    iconL.start, iconL.end);
  makeQuad(mesh, dtl-right, dtl, dtl-right+down, dtl+down,
    reverseL.start, reverseL.end);
}

void makeIBeam(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along, glm::vec3 up,
    float innerSize, bool includeTop, bool includeBottom, glm::vec3 xs) {

  glm::vec3 internal = normalize(right)*innerSize;
  glm::vec3 unitUp = normalize(up)*0.25f;
  glm::vec3 innerUp = up-unitUp;
  makeAngledCube(mesh, loc, right, along, unitUp, true, xs);
  makeAngledCube(mesh, loc+innerUp, right, along, unitUp, includeTop, xs);
  makeAngledCube(mesh, loc+right*.5f-internal*.5f, internal,
    along, up, false, xs);
  if (includeBottom) {
    makeQuad(mesh, loc, loc+right, loc+along, loc+right+along, xs, xs);
    makeQuad(mesh, loc+innerUp, loc+innerUp+right,
        loc+innerUp+along, loc+innerUp+right+along, xs, xs);
  }
}

