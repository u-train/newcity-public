#include "util.hpp"
#include "item.hpp"
#include "string_proxy.hpp"

#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

item iclamp(item a, item b, item c) {
  if (a < b) return b;
  if (a > c) return c;
  return a;
}

bool removeFromVector(std::vector<item>* v, item n) {
  bool found = false;
  for (int i = v->size()-1; i >= 0; i--) {
    if (v->at(i) == n) {
      v->erase(v->begin()+i);
      found = true;
    }
  }
  return found;
}

bool isInVector(std::vector<item>* v, item n) {
  for (int i = 0; i < v->size(); i ++) {
    if (v->at(i) == n) return true;
  }
  return false;
}

// Function to linearly interpolate between a0 and a1
// Weight w should be in the range [0.0, 1.0]
float lerp(float a0, float a1, float w) {
  return (1.0 - w)*a0 + w*a1;
}

glm::vec3 lerp(glm::vec3 a0, glm::vec3 a1, float w) {
  return (1.0f - w)*a0 + w*a1;
}

float nonZero(float val) {
  return val == 0 ? 1 : val;
}

float getSqr(float val) {
  return val * val;
}

float distanceSqrd(glm::vec3 v1, glm::vec3 v2) {
  float x, y, z;
  x = v1.x - v2.x;
  y = v1.y - v2.y;
  z = v1.z - v2.z;
  return x*x + y*y + z*z;
}

float distance2D(glm::vec3 v1, glm::vec3 v2) {
  float x, y;
  x = v1.x - v2.x;
  y = v1.y - v2.y;
  return sqrt(x*x + y*y);
}

float distance2DSqrd(glm::vec2 v1, glm::vec2 v2) {
  float x, y;
  x = v1.x - v2.x;
  y = v1.y - v2.y;
  return x*x + y*y;
}

float distance2DSqrd(glm::vec3 v1, glm::vec3 v2) {
  float x, y;
  x = v1.x - v2.x;
  y = v1.y - v2.y;
  return x*x + y*y;
}

float lengthSqrd(glm::vec3 v) {
  return v.x*v.x + v.y*v.y + v.z*v.z;
}

float vecDistance(glm::vec3 v1, glm::vec3 v2) {
  float x, y, z;
  x = v1.x - v2.x;
  y = v1.y - v2.y;
  z = v1.z - v2.z;
  return sqrt(x*x + y*y + z*z);
}

glm::vec3 zNormal(glm::vec3 v1) {
  return glm::vec3(v1.y, -v1.x, 0);
}

glm::vec3 uzNormal(glm::vec3 v1) {
  return normalize(glm::vec3(v1.y, -v1.x, 0));
}

void printVec(glm::vec3 vec) {
  printf("(%f, %f, %f)\n", vec.x, vec.y, vec.z);
}

void printMatrix(glm::mat4 matrix) {
  const float *pSource = (const float*)glm::value_ptr(matrix);
  for (int i = 0; i < 4; i ++) {
    if (i == 0) {
      printf("(");
    } else {
      printf(" ");
    }
    printf("(");
    for (int j = 0; j < 4; j++) {
      printf("%f", pSource[i*4 + j]);
      if (j == 3) {
        printf(")");
      } else {
        printf(", ");
      }
    }
    if (i == 3) {
      printf(")");
    }
    printf("\n");
  }
}

float pointLineDistance(const glm::vec3 p, const Line l) {
  return sqrt(pointLineDistanceSqrd(p, l));
}

float pointLineDistanceSqrd(const glm::vec3 p, const Line l) {
  /*
   * DON'T DELETE
   *
   * Use this commented code for reference, unrolled high-perf version below
   *
  // Return minimum distance between line segment vw and point p
  const float l2 = distanceSqrd(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
  if (l2 == 0.0) return vecDistance(p, v);   // v == w case
  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line. 
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  // We clamp t from [0,1] to handle points outside the segment vw.
  const float t = std::max(0.f, std::min(1.f, dot(p - v, w - v) / l2));
  const glm::vec3 projection = v + t * (w - v);  // Projection falls on the segment
  return vecDistance(p, projection);
  */

  const float px = p.x;
  const float py = p.y;
  const float pz = p.z;

  const float vx = l.start.x;
  const float vy = l.start.y;
  const float vz = l.start.z;

  const float wx = l.end.x;
  const float wy = l.end.y;
  const float wz = l.end.z;

  const float wvx = wx - vx;
  const float wvy = wy - vy;
  const float wvz = wz - vz;

  const float pvx = px - vx;
  const float pvy = py - vy;
  const float pvz = pz - vz;

  const float l2 = wvx*wvx + wvy*wvy + wvz*wvz;

  if (l2 == 0.0f) return pvx*pvx + pvy*pvy + pvz*pvz;

  const float d = pvx*wvx + pvy*wvy + pvz*wvz;
  const float dn = d / l2;
  const float t = dn < 0.f ? 0.f : dn > 1.f ? 1.f : dn;

  const float jx = vx + t*wvx;
  const float jy = vy + t*wvy;
  const float jz = vz + t*wvz;

  const float jpx = jx - px;
  const float jpy = jy - py;
  const float jpz = jz - pz;

  return jpx*jpx + jpy*jpy + jpz*jpz;
}

float pointLineDistance2DSqrd(const glm::vec3 p, const Line l) {
  const float px = p.x;
  const float py = p.y;

  const float vx = l.start.x;
  const float vy = l.start.y;

  const float wx = l.end.x;
  const float wy = l.end.y;

  const float wvx = wx - vx;
  const float wvy = wy - vy;

  const float pvx = px - vx;
  const float pvy = py - vy;

  const float l2 = wvx*wvx + wvy*wvy;

  if (l2 == 0.0f) return pvx*pvx + pvy*pvy;

  const float d = pvx*wvx + pvy*wvy;
  const float dn = d / l2;
  const float t = dn < 0.f ? 0.f : dn > 1.f ? 1.f : dn;

  const float jx = vx + t*wvx;
  const float jy = vy + t*wvy;

  const float jpx = jx - px;
  const float jpy = jy - py;

  return jpx*jpx + jpy*jpy;
}

float pointRayDistanceSqrd(const glm::vec3 p, const Line l) {
  const float px = p.x;
  const float py = p.y;
  const float pz = p.z;

  const float vx = l.start.x;
  const float vy = l.start.y;
  const float vz = l.start.z;

  const float wx = l.end.x;
  const float wy = l.end.y;
  const float wz = l.end.z;

  const float wvx = wx - vx;
  const float wvy = wy - vy;
  const float wvz = wz - vz;

  const float pvx = px - vx;
  const float pvy = py - vy;
  const float pvz = pz - vz;

  const float l2 = wvx*wvx + wvy*wvy + wvz*wvz;

  if (l2 == 0.0f) return pvx*pvx + pvy*pvy + pvz*pvz;

  const float d = pvx*wvx + pvy*wvy + pvz*wvz;
  const float t = d / l2;

  const float jx = vx + t*wvx;
  const float jy = vy + t*wvy;
  const float jz = vz + t*wvz;

  const float jpx = jx - px;
  const float jpy = jy - py;
  const float jpz = jz - pz;

  return jpx*jpx + jpy*jpy + jpz*jpz;
}

glm::vec3 nearestPointOnLine(glm::vec3 p, Line l) {

  /*
   * DON'T DELETE
   *
   * Use this commented code for reference, unrolled high-perf version below
   *
  glm::vec3 v = l.start, w = l.end;
  // Return minimum distance between line segment vw and point p
  const float l2 = distanceSqrd(v, w);  // i.e. |w-v|^2 -  avoid a sqrt
  if (l2 == 0.0) return v;   // v == w case
  const float t = std::max(0.f, std::min(1.f, dot(p - v, w - v) / l2));
  const glm::vec3 projection = v + t * (w - v);  // Projection falls on the segment
  return projection;
  */

  const float px = p.x;
  const float py = p.y;
  const float pz = p.z;

  const float vx = l.start.x;
  const float vy = l.start.y;
  const float vz = l.start.z;

  const float wx = l.end.x;
  const float wy = l.end.y;
  const float wz = l.end.z;

  const float wvx = wx - vx;
  const float wvy = wy - vy;
  const float wvz = wz - vz;

  const float pvx = px - vx;
  const float pvy = py - vy;
  const float pvz = pz - vz;

  const float l2 = wvx*wvx + wvy*wvy + wvz*wvz;

  if (l2 == 0.0f) return l.start;

  const float d = pvx*wvx + pvy*wvy + pvz*wvz;
  const float dn = d / l2;
  const float t = dn < 0.f ? 0.f : dn > 1.f ? 1.f : dn;

  const float jx = vx + t*wvx;
  const float jy = vy + t*wvy;
  const float jz = vz + t*wvz;

  return glm::vec3(jx, jy, jz);
}

glm::vec3 pointOfIntersection(Line l0, Line l1) {

  glm::vec3 a0 = l0.start;
  glm::vec3 a1 = l0.end;
  glm::vec3 b0 = l1.start;
  glm::vec3 b1 = l1.end;

  glm::vec3 A = a1 - a0;
  glm::vec3 B = b1 - b0;

  float magA = length(A);
  float magB = length(B);

  glm::vec3 _A = A / magA;
  glm::vec3 _B = B / magB;

  glm::vec3 crossVec = cross(_A, _B);
  float denom = lengthSqrd(crossVec);

  // If lines are parallel (denom=0), return null
  if (denom == 0) {
    return glm::vec3(-1, -1, -1);
  }

  // Lines criss-cross: Calculate the projected closest points
  glm::vec3 t = (b0 - a0);
  float detA = determinant(glm::mat3(t, _B, crossVec));

  float t0 = glm::clamp(detA/denom, 0.f, magA);

  glm::vec3 pA = a0 + (_A * t0); // Projected closest point on segment A

  return pA;
}

bool line2Dintersect(Line l0, Line l1) {
  const float x1 = l0.start.x;
  const float y1 = l0.start.y;
  const float x2 = l0.end.x;
  const float y2 = l0.end.y;
  const float x3 = l1.start.x;
  const float y3 = l1.start.y;
  const float x4 = l1.end.x;
  const float y4 = l1.end.y;

  const float x12 = x1 - x2;
  const float y12 = y1 - y2;
  const float x34 = x3 - x4;
  const float y34 = y3 - y4;

  const float denom = x12*y34 - y12*x34;
  if (denom == 0) {
    return false;
  }

  const float x13 = x1 - x3;
  const float y13 = y1 - y3;

  const float t = x13*y34 - y13*x34;
  const float u = x13*y12 - y13*x12;

  return denom < 0 ?
    t <= 0 && t >= denom && u <= 0 && u >= denom :
    t >= 0 && t <= denom && u >= 0 && u <= denom;
}

float lineDistance2D(Line l0, Line l1) {
  l0.start.z = 0;
  l1.start.z = 0;
  l0.end.z = 0;
  l1.end.z = 0;
  return lineDistance(l0, l1);
}

float lineDistance(Line l0, Line l1) {

  glm::vec3 a0 = l0.start;
  glm::vec3 a1 = l0.end;
  glm::vec3 b0 = l1.start;
  glm::vec3 b1 = l1.end;

  glm::vec3 A = a1 - a0;
  glm::vec3 B = b1 - b0;

  float magA = length(A);
  float magB = length(B);

  glm::vec3 _A = A / magA;
  glm::vec3 _B = B / magB;

  glm::vec3 crossVec = cross(_A, _B);
  float denom = lengthSqrd(crossVec);

  // If lines are parallel (denom=0) test if lines overlap.
  // If they don't overlap then there is a closest point solution.
  // If they do overlap, there are infinite closest positions,
  // but there is a closest distance
  if (denom == 0) {
    float d0 = dot(_A,(b0-a0));

    // Overlap only possible with clamping
    float d1 = dot(_A,(b1-a0));

    // Is segment B before A?
    if (d0 <= 0 && 0 >= d1) {
      if (abs(d0) < abs(d1)) {
        return length(a0-b0);
      }
      return length(a0-b1);
    } else if (d0 >= magA && magA <= d1) { // Is segment B after A?
      if (abs(d0) < abs(d1)) {
        return length(a1-b0);
      }
      return length(a1-b1);
    }
    // Segments overlap, return distance between parallel segments
    return length(((d0*_A)+a0)-b0);
  }

  // Lines criss-cross: Calculate the projected closest points
  glm::vec3 t = (b0 - a0);
  float detA = determinant(glm::mat3(t, _B, crossVec));
  float detB = determinant(glm::mat3(t, _A, crossVec));

  float t0 = detA/denom;
  float t1 = detB/denom;

  glm::vec3 pA = a0 + (_A * t0); // Projected closest point on segment A
  glm::vec3 pB = b0 + (_B * t1); // Projected closest point on segment B

  // Clamp projections
  if (t0 < 0) {
    pA = a0;
  } else if (t0 > magA) {
    pA = a1;
  }

  if (t1 < 0) {
    pB = b0;
  } else if (t1 > magB) {
    pB = b1;
  }

  // Clamp projection A
  if (t0 < 0 || t0 > magA) {
    float dotProd = dot(_B,(pA-b0));
    if (dotProd < 0) {
      dotProd = 0;
    } else if (dotProd >magB) {
      dotProd = magB;
    }
    pB = b0 + (_B * dotProd);
  }

  // Clamp projection B
  if (t1 < 0 || t1 > magB) {
    float dotProd = dot(_A,(pB-a0));
    if (dotProd < 0) {
      dotProd = 0;
    } else if (dotProd > magA) {
      dotProd = magA;
    }
    pA = a0 + (_A * dotProd);
  }

  return length(pA-pB);
}

glm::vec3 lineAtZ(Line l, float z) {
  glm::vec3 along = l.end - l.start;
  float mag = (z - l.start.z) / along.z;
  return l.start + along*mag;
}

static item randomSeed = 1;

item nextRand(item* randomSeed) {
  // xorshift* 32-bit
  item x = *randomSeed;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  *randomSeed = x;
  return x < 0 ? -x : x;

  //*randomSeed = 1103515245*(*randomSeed) + 12345;
}
item nextRand() {
  return nextRand(&randomSeed);
}

item randItem(item* randomSeed, item num) {
  return nextRand(randomSeed) % num;
}
item randItem(item num) {
  if (num == 0) return 0;
  return nextRand() % num;
}

item randItem(item* randomSeed, item a, item b) {
  item max = std::max(a, b);
  item min = std::min(a, b);
  item diff = max - min;
  return randItem(randomSeed, diff) + min;
}

item randItem(item a, item b) {
  return randItem(&randomSeed, a, b);
}

item randInSet(std::set<item>* s, item num) {
  int size = s->size();
  if (size == 0) {
    return 0;
  } else if (size == 1) {
    return *(s->begin());
  }

  item ndx = rand()%size;
  auto it = std::begin(*s);
  std::advance(it, ndx); //NOTE: O(n)
  return *it;

  /*
  item ndx = rand()%num;
  pair<std::set<item>::iterator, bool> result = s->insert(ndx);
  item first = *(result.first);
  //SPDLOG_INFO("randInSet");//, result.second, size);
  if (result.second) {
    item res = *(result.first++);
    s->erase(ndx);
    return res;
  } else {
    return *(result.first);
  }
  */
}

const float mf = INT_MAX;
float randFloat(item* randomSeed) {
  uint32_t us = nextRand(randomSeed);
  return us/mf;
}
float randFloat() {
  return randFloat(&randomSeed);
}

float randFloat(item* randomSeed, float a, float b) {
  return lerp(a, b, randFloat(randomSeed));
}
float randFloat(float a, float b) {
  return randFloat(&randomSeed, a, b);
}

item randRound(float val) {
  item result = std::floor(val);
  float fraction = val - result;
  if (randFloat() < fraction) result ++;
  return result;
}

float slope(Line line) {
  float x = line.end.x - line.start.x;
  float y = line.end.y - line.start.y;
  float z = abs(line.end.z - line.start.z);
  return z / sqrt(x*x + y*y);
}

// orig and dir defines the ray. v0, v1, v2 defines the triangle.
// returns the distance from the ray origin to the intersection or 0.
float triangle_intersection(Line line,
  const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2) {

    glm::vec3 orig = line.start;
    glm::vec3 dir = line.end - orig;
    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;
    // Calculate planes normal std::vector
    glm::vec3 pvec = cross(dir, e2);
    float det = dot(e1, pvec);

    // Ray is parallel to plane
    if (det < 1e-8 && det > -1e-8) {
        return 0;
    }

    float inv_det = 1 / det;
    glm::vec3 tvec = orig - v0;
    float u = dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        return 0;
    }

    glm::vec3 qvec = cross(tvec, e1);
    float v = dot(dir, qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return 0;
    }
    return dot(e2, qvec) * inv_det;
}

bool boxLineIntersect(Line box, Line ml) {
  glm::vec3 t00 = box.start;
  glm::vec3 t01 = glm::vec3(box.start.x, box.end.y, box.start.z);
  glm::vec3 t10 = glm::vec3(box.end.x, box.start.y, box.end.z);
  glm::vec3 t11 = box.end;
  float ti0 = triangle_intersection(ml, t00, t01, t10);
  float ti1 = triangle_intersection(ml, t01, t11, t10);
  float ru = ti0 > ti1 ? ti0 : ti1;
  return ru > 0;
}

void free_proxy(void* ptr) {
  free(ptr);
}

glm::vec3 linePlaneIntersect(Line ray, glm::vec3 p0, glm::vec3 n) {
  glm::vec3 l0 = ray.start;
  glm::vec3 l = ray.end-l0;
  float denom = dot(l, n);
  if (abs(denom) < 1e-6) {
    return glm::vec3(-1,-1,-1);
  }
  float t = dot((p0-l0), n)/denom;
  return l0+t*l;
}

bool isInDim(glm::vec2 loc, Line dim) {
  return loc.x >= dim.start.x && loc.x <= dim.start.x + dim.end.x &&
    loc.y >= dim.start.y && loc.y <= dim.start.y + dim.end.y;
}

bool validate(float f) {
  return f == f && !std::isnan(f) && std::isfinite(f);
}

bool validate(glm::vec3 v) {
  return validate(v.x) && validate(v.y) && validate(v.z);
}

bool validate(Line l) {
  return validate(l.start) && validate(l.end);
}

static float thousand = 1000;
static float million = thousand*thousand;
static float billion = million*thousand;
static float trillion = billion*thousand;

char* formatFloat(float val) {
  const char* negSign = "";
  if (val < -0.00001f) {
    negSign = "-";
    val = -val;
  }

  if (val >= trillion*10) {
    return sprintf_o("%s%3dT", negSign, (int)(val/trillion));
  } else if (val >= trillion) {
    return sprintf_o("%s%.1fT", negSign, val/trillion);
  } else if (val >= billion*10) {
    return sprintf_o("%s%3dB", negSign, (int)(val/billion));
  } else if (val >= billion) {
    return sprintf_o("%s%.1fB", negSign, val/billion);
  } else if (val >= million*10) {
    return sprintf_o("%s%3dM", negSign, (int)(val/million));
  } else if (val >= million) {
    return sprintf_o("%s%.1fM", negSign, val/million);
  } else if (val >= thousand*10) {
    return sprintf_o("%s%3dK", negSign, (int)(val/thousand));
  } else if (val >= thousand) {
    return sprintf_o("%s%.1fK", negSign, val/thousand);
  } else if (val >= 100) {
    return sprintf_o("%s%3d", negSign, (int)val);
  } else if (val >= 10) {
    return sprintf_o("%s%.1f", negSign, val);
  } else if (val >= 1) {
    return sprintf_o("%s%.2f", negSign, val);
  } else {
    return sprintf_o("%s%.3f", negSign, val);
  }
}

