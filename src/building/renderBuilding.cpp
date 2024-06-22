#include "renderBuilding.hpp"

#include "building.hpp"
#include "design.hpp"
#include "designPackage.hpp"
#include "statue.hpp"

#include "../box.hpp"
#include "../color.hpp"
#include "../draw/entity.hpp"
#include "../draw/texture.hpp"
#include "../icons.hpp"
#include "../import/mesh-import.hpp"
#include "../game/game.hpp"
#include "../land.hpp"
#include "../parts/designConfigPanel.hpp"
#include "../parts/toolbar.hpp"
#include "../plan.hpp"
#include "../renderUtils.hpp"
#include "../selection.hpp"
#include "../string.hpp"
#include "../string_proxy.hpp"
#include "../util.hpp"
#include "../weather.hpp"
#include "../zone.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>
using glm::clamp;
using glm::cos;
using glm::sin;

#include <algorithm>

//Building render
const float paveFlare = 4;
/*
vec4 buildingColor = glm::vec4(.902f, .765f, .733f, 1.0f);
vec4 roofColor = glm::vec4(.376f, .208f, .169f, 1.0f);
vec4 trimColor = glm::vec4(.376f, .208f, .169f, 1.0f);
*/
static item handlesEntity = 0;
static item bordersEntity = 0;
static item designTextEntity = 0;
//const float simpleDistance = 100;

static const glm::vec3 xp = glm::vec3(0.5, 0.0, 0);
//static const glm::vec3 xwall_s = glm::vec3(0/16.f, 0, 1);
//static const glm::vec3 xwall_e = glm::vec3(1/16.f, 1, 1);
static const glm::vec3 xwall_s = glm::vec3(5.f/16.f, 0.5, 0);
static const glm::vec3 xwall_e = glm::vec3(5.f/16.f, 0.5, 0);
static const glm::vec3 xbwall_s = glm::vec3(8/16.f, 0, 0);
static const glm::vec3 xbwall_e = glm::vec3(9/16.f, 1, 0);
//static const glm::vec3 xroof_s = glm::vec3(3.1/16.f, 0, 1);
//static const glm::vec3 xroof_e = glm::vec3(3.9/16.f, 1, 1);
static const glm::vec3 xroof_s = glm::vec3(0., 0.00, 0);
static const glm::vec3 xroof_e = glm::vec3(1., 0.25, 0);
static const glm::vec3 xroof_ss = glm::vec3(0, 0.00, 1);
static const glm::vec3 xroof_es = glm::vec3(1., 0.25, 1);
static item buildingRenderSeed = 101;
static float lastLowTide = -1;
static float lastHighTide = -1;
static bool wasAquatic = false;

static const float xwindow[] = {
  1/16.f, // NoZone
  1/16.f, // Residential
  5/16.f, // Retail
  7/16.f, // Farm
  9/16.f, // Government
  5/16.f, // Office
  7/16.f, // Factory
  1/16.f, // Mixed Use
};

static const float xdoor[] = {
  2/16.f, // NoZone
  2/16.f, // Residential
  4/16.f, // Retail
  6/16.f, // Farm
  8/16.f, // Government
  4/16.f, // Office
  6/16.f, // Factory
  2/16.f, // Mixed Use
};

const float treeHeight = 15;
const float treeWidth = 10;
const float treeBelly = 2;
const double sectionLength = 4;

static std::vector<float> decoCull;
static std::vector<item> issueIconMeshes;

void resetBuildingRender() {
  issueIconMeshes.clear();
  decoCull.clear();
}

glm::vec3 adjustRoofTex(glm::vec3 loc, glm::vec3 br, glm::vec3 axis) {
  glm::vec3 along = loc - br;
  float x = dot(along, axis);
  float y = dot(along, glm::vec3(axis.y, -axis.x, 0));
  y = -length(glm::vec3(0, y, along.z));
  glm::vec3 result = glm::vec3(0, 0.25, 0) + glm::vec3(x,y,0)/16.f;
  result.y = glm::clamp(result.y, 0.f, 0.25f);
  result.z = 1;
  return result;
}

void makeRoofQuad(Mesh* mesh, glm::vec3 br, glm::vec3 bl, glm::vec3 tr, glm::vec3 tl) {
  glm::vec3 xbr = glm::vec3(0,0.25,1);
  glm::vec3 axis = normalize(bl - br);
  glm::vec3 xtr = adjustRoofTex(tr, br, axis);
  glm::vec3 xbl = adjustRoofTex(bl, br, axis);
  glm::vec3 xtl = adjustRoofTex(tl, br, axis);
  makeQuad(mesh, tr, br, tl, bl, xtr, xbr, xtl, xbl);
}

void makeWallNew(Mesh* mesh, glm::dvec3 loc, glm::dvec3 along, double height,
  item zone, bool simple) {

  const double texSize1 = 4.;
  const double texSize4 = 16.;

  double drop = 0.5;
  double l = length(along)/sectionLength;
  double h = height/sectionLength;
  double lz = loc.z;
  glm::dvec3 ualong = normalize(along)*sectionLength;

  if (lz <= 0) {
    loc -= glm::vec3(0,0,sectionLength*drop);
    h += drop;
  }

  for (double y = 0; y < h; y += texSize1) {
    double ya = std::min(texSize1, h-y);
    if (ya < 0.1) continue;
    bool lastPart = h-y-texSize1 < 0.1;
    bool firstPart = y <= 0 && lz <= sectionLength;

    glm::dvec3 start = loc + glm::dvec3(0,0,sectionLength*y);
    glm::dvec3 sup = glm::vec3(0,0,sectionLength*ya);
    glm::dvec3 start_up = start+sup;
    //double tx_y = ya/48./16.f;
    double tx_ys = firstPart ? (texSize4-ya)/texSize4 :
      lastPart ? texSize1/texSize4 : (texSize1*3.-ya)/texSize4;
    double tx_ye = firstPart ? 1. :
      lastPart ? (texSize1+ya)/texSize4 : texSize1*3./texSize4;
    double tx_x = int(l)/texSize1;
    if (tx_x <= 0) tx_x = .5/texSize1;
    double tx_b = int(l*.5+.25)/texSize1;
    //double tx_b = tx_x*.5;
    double tx_xs = 0.5-tx_b;
    if (randFloat(&buildingRenderSeed) < 0.5) tx_xs ++;
    double tx_xe = tx_xs + tx_x;
    glm::dvec3 tx_s = glm::dvec3(tx_xs, tx_ys, 0.);
    glm::dvec3 tx_e = glm::dvec3(tx_xe, tx_ye, 0.);

    makeQuad(mesh, start_up, start_up+along, start, start+along, tx_s, tx_e);

    /*
    for (double x = 0; x < l; x += texSize1) {
      double 
      makeQuad(mesh, start_up, start_up+along, start, start+along, tx_s, tx_e);
    }
    */
  }
}

void makeWall(Mesh* mesh, glm::dvec3 loc, glm::dvec3 along, double height,
  item zone, bool simple) {

  double l = length(along)/sectionLength;
  double h = height/sectionLength + 2;
  glm::dvec3 ualong = normalize(along)*sectionLength;
  loc -= glm::vec3(0,0,sectionLength*2);
  item zoneAdj = zone == OfficeZone ? RetailZone :
    zone == FactoryZone ? FarmZone :
    zone == MixedUseZone ? ResidentialZone : zone;

  for (int x = 0; x < l; x+=4) {
    for (int b = 0; b < (simple?1:2); b++) {
      double xa = std::min(4., l-x);
      double ya = simple ? h : (b ? h-4 : std::min(4., h));
      if (ya < 0.01) {
        continue;
      }

      glm::dvec3 start = loc + ualong*double(x) + glm::dvec3(0,0,sectionLength*4*b);
      glm::dvec3 salong = ualong*xa;
      glm::dvec3 sup = glm::vec3(0,0,sectionLength*ya);
      float texZone = (zoneAdj-1)*0.25 + (b||simple ? 0.125 : 0);
      float yfudge = b ? (ya-int(ya))*0.125 : 0;
      glm::dvec3 texStart = glm::dvec3(texZone, 1-ya*0.25-yfudge, 1);
      glm::dvec3 texEnd = glm::dvec3(texZone + xa/32, 1-yfudge, 1);

      makeQuad(mesh, start+sup, start+sup+salong, start, start+salong,
        texStart, texEnd);
    }
  }
}

void makeBuildingCube(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along,
  float height, item zone, bool simple) {

  makeWallNew(mesh, loc, right, height, zone, simple);
  makeWallNew(mesh, loc+along+right, -right, height, zone, simple);
  makeWallNew(mesh, loc+along, -along, height, zone, simple);
  makeWallNew(mesh, loc+right, along, height, zone, simple);

  if (loc.z > 0) {
    makeQuad(mesh, loc, loc+right, loc+along, loc+along+right, xp, xp);
  }
}

void makeFlat(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along, float height,
    item zone, float slope, bool simple) {

  slope *= 1.1;
  glm::vec3 up = glm::vec3(0,0,height);
  glm::vec3 s = loc-right*.5f;
  glm::vec3 ru = normalize(up) * slope;
  glm::vec3 rs = s + up - ru;
  glm::vec3 rb = s + up - ru*1.01f;
  glm::vec3 ir = normalize(right) * (simple?2.f:1.f) * slope;
  glm::vec3 ia = normalize(along) * (simple?2.f:1.f) * slope;

  /*
  //Air Conditioner
  if (!simple && length(along) > 12.f*slope && length(right) > 6.f*slope) {
    makeAngledCube(mesh, rs+ia*5.f+ir*5.f, ia*4.f, -ir*2.f, ru*2.f, true, xp);
    makeAngledCube(mesh, rs+ia*6.f+ir*6.f, ia*2.f, -ir, ru, true, xp);

    //Chimney
    makeAngledCube(mesh, rs+along-ia*4.f+ir*2.f, ir, ia, ru*2.f, true, xp);
  }
  */

  if (simple) {
    makeQuad(mesh, rs, rs+along, rs+right, rs+right+along, xroof_ss, xroof_es);
    makeQuad(mesh, rs+ru+ir+ia, rs+ru+along+ir-ia,
      rs+ru+right-ir+ia, rs+ru+right+along-ir-ia, xp, xp);

  } else {
    makeQuad(mesh, rs+ir+ia, rs+along+ir-ia,
      rs+right-ir+ia, rs+right+along-ir-ia, xp, xp);

    makeQuad(mesh, rs+ru, rs+along+ru,
      rs+ir+ia+ru, rs+along+ir-ia+ru, xroof_ss, xroof_es);
    makeQuad(mesh, rs+right+ru, rs+ru,
      rs+right-ir+ia+ru, rs+ir+ia+ru, xroof_ss, xroof_es);
    makeQuad(mesh, rs+along+ru, rs+along+right+ru,
      rs+along+ir-ia+ru, rs+right+along-ir-ia+ru, xroof_ss, xroof_es);
    makeQuad(mesh, rs+right+along+ru, rs+right+ru,
      rs+right+along-ir-ia+ru, rs+right-ir+ia+ru, xroof_ss, xroof_es);

    makeQuad(mesh, rs+ir+ia+ru, rs+along+ir-ia+ru,
       rb+ir+ia, rb+along+ir-ia, xroof_s, xroof_e);
    makeQuad(mesh, rs+right-ir+ia+ru, rs+ir+ia+ru,
      rb+right-ir+ia, rb+ir+ia, xroof_s, xroof_e);
    makeQuad(mesh, rs+along+ir-ia+ru, rs+right+along-ir-ia+ru,
      rb+along+ir-ia, rb+right+along-ir-ia, xroof_s, xroof_e);
    makeQuad(mesh, rs+right+along-ir-ia+ru, rs+right-ir+ia+ru,
      rb+right+along-ir-ia, rb+right-ir+ia, xroof_s, xroof_e);
  }
}

void makeHip(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along, float height,
    item zone, float slope, bool simple) {

  glm::vec3 up = glm::vec3(0,0,height);
  float lright = length(right);
  float lalong = length(along);

  loc -= right*.5f;
  if (lright > lalong) {
    glm::vec3 temp = right;
    right = along;
    along = -temp;
    loc -= along;
  }

  glm::vec3 rs = loc+up;
  glm::vec3 rc = rs+right*.5f;
  float roofHeight = length(right)*slope;
  glm::vec3 pitch = glm::vec3(0,0,roofHeight);
  glm::vec3 in = normalize(along) * roofHeight;
  glm::vec3 rp0 = rc+in+pitch;
  glm::vec3 rp1 = rc+along-in+pitch;
  makeRoofQuad(mesh, rs, rs+along, rp0, rp1);
  makeRoofQuad(mesh, rs+along+right, rs+right, rp1, rp0);
  glm::vec3 xr0 = glm::vec3(0,0.25,1);
  glm::vec3 xr1 = glm::vec3(0.5,0,1);
  glm::vec3 xr2 = glm::vec3(1,0.25,1);
  makeTriangle(mesh, rs, rp0, rs+right, xr0, xr1, xr2);
  makeTriangle(mesh, rs+along+right, rp1, rs+along, xr0, xr1, xr2);

  /*
  //Chimney
  if (!simple) {
    glm::vec3 ia = normalize(along);
    glm::vec3 ir = normalize(right);
    glm::vec3 ru = glm::vec3(0,0,1);
    makeAngledCube(mesh, rs+along-ia*4.f+ir*1.f, ir, ia, ru*2.f, true, xp);
  }
  */
}

void makeSlant(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along, float height,
    item zone, float slope, bool simple) {

  loc += glm::vec3(0,0,height) - right*.5f;

  glm::vec3 ia = normalize(along);
  glm::vec3 ir = normalize(right);
  glm::vec3 ru = glm::vec3(0,0,1);

  glm::vec3 roofUp = glm::vec3(0,0, length(right)*slope);
  glm::vec3 locAlong = loc+along;
  makeRoofQuad(mesh,
      locAlong+right+roofUp,
      loc+right+roofUp,
      locAlong,
      loc
      );

  makeTriangle(mesh, loc+right, loc, loc+right+roofUp, xwall_s);
  makeTriangle(mesh, locAlong, locAlong+right,
      locAlong+right+roofUp, xwall_s);
  makeQuad(mesh, loc+right+along, loc+right,
      loc+right+roofUp+along, loc+right+roofUp, xwall_s, xwall_e);

  /*
  //Chimney
  if (!simple) {
    makeAngledCube(mesh, loc+ia*4.f+ir*2.f, ir, ia, ru*2.f, true, xp);
  }
  */
}

void makeBarrel(Mesh* mesh, glm::vec3 loc, glm::vec3 right, glm::vec3 along, float height,
    int segments, item zone, float slope, bool simple) {

  float theta = pi_o/segments;
  glm::vec3 ys[8];
  glm::vec3 zs[8];
  glm::vec3 up = glm::vec3(0,0,height);
  float roofHeight = length(right)*slope;
  for (int i=0; i <= segments; i++) {
    ys[i] = right * cos(theta*i) * 0.5f;
    zs[i] = glm::vec3(0, 0, sin(theta*i)*roofHeight);
  }
  for (int i=0; i < segments; i++) {
    glm::vec3 loc0 = loc+ys[i  ]+up;
    glm::vec3 loc1 = loc+ys[i+1]+up;
    glm::vec3 rloc0 = loc0+zs[i  ];
    glm::vec3 rloc1 = loc1+zs[i+1];
    makeQuad(mesh, rloc0, loc0, rloc1, loc1, xwall_s, xwall_e);
    makeQuad(mesh, loc0+along, rloc0+along, loc1+along, rloc1+along,
        xwall_s, xwall_e);
    makeRoofQuad(mesh, rloc0+along, rloc0, rloc1+along, rloc1);
  }

  glm::vec3 e0 = ys[0]+loc;
  glm::vec3 re0 = e0 + zs[0]+up;
  glm::vec3 e1 = ys[segments]+loc;
  glm::vec3 re1 = e1 + zs[segments]+up;
  //makeQuad(mesh, e0+along, e0, re0+along, re0);
  //makeQuad(mesh, e1, e1+along, re1, re1+along);

  glm::vec3 ia = normalize(along);
  glm::vec3 ir = normalize(right);
  glm::vec3 ru = glm::vec3(0,0,1);

  /*
  //Chimney
  if (!simple) {
    makeAngledCube(mesh, re0+along-ia*4.f-ir*2.f, ir, ia, ru*2.f, true, xp);
  }
  */
}

void renderStructure(Mesh* mesh, Structure* structure, item zone,
    bool simple) {
  glm::vec3 loc = structure->location;
  glm::vec3 size = structure->size;
  float slope = structure->roofSlope;
  float cangle = cos(structure->angle);
  float sangle = sin(structure->angle);
  glm::vec3 right = glm::vec3(-cangle, sangle, 0) * size.x;
  glm::vec3 along = glm::vec3(sangle, cangle, 0) * size.y;
  item roofType = structure->roofType;
  bool body = roofType >= 0;
  if (roofType < 0) {
    roofType = -roofType-1;
  }

  if (simple && size.x * size.y < 100) {
    return;
  }

  if (body) {
    makeBuildingCube(mesh, loc-right*.5f, right, along, size.z, zone, simple);
  }

  switch(roofType) {
    case GableRoof: {
      makeBarrel(mesh, loc, right, along, size.z, 2, zone, slope, simple);
    } break;
    case HipRoof: {
      makeHip(mesh, loc, right, along, size.z, zone, slope, simple);
    } break;
    case FlatRoof: {
      makeFlat(mesh, loc, right, along, size.z, zone, slope, simple);
    } break;
    case BarrelRoof: {
      makeBarrel(mesh, loc, right, along, size.z, 6, zone, slope, simple);
    } break;
    case SlantRoof: {
      makeSlant(mesh, loc, right, along, size.z, zone, slope, simple);
    } break;
    case GambrelRoof: {
      makeBarrel(mesh, loc, right, along, size.z, 4, zone, slope, simple);
    } break;
    default: {
    } break;
  }
}

void setDecoCull(float dist, item ndx) {
  decoCull[ndx] += dist;
  //if (decoCull[ndx] < dist) {
  //}
}

glm::vec3 maxPoint(glm::vec3 current, glm::vec3 addition) {
  if (abs(addition.y)*2 > current.y) {
    current.y = abs(addition.y)*2;
  }
  if (addition.x > current.x) {
    current.x = addition.x;
  }
  if (addition.z > current.z) {
    current.z = addition.z;
  }
  return current;
}

glm::vec3 maxPoint(glm::vec3 current, Deco* deco) {
  current = maxPoint(current, deco->location);

  item type = deco->decoType;
  if (type < numLegacyDecoTypes) return current;
  DecoType* decoType = getDecoType(deco->decoType);
  item importNdx = decoType->meshImport;

  if (deco->decoType == statueDecoType()) return current;

  if (importNdx <= 0) return current;
  MeshImport* import = getMeshImport(importNdx);
  if (!(import->flags & _meshImportComplete)) {
    loadMeshImport(decoType->meshImport);
  }

  // Gotta go get that z
  float scaleZ = (decoType->flags & _decoScaleZ) ? deco->scale : 1;
  for (int s = 0; s < 2; s++) {
    item meshNdx = s ? import->mesh : import->simpleMesh;
    if (meshNdx <= 0) continue;
    Mesh* mesh = getMesh(meshNdx);
    for (int i = 0; i < mesh->numTriangles; i++) {
      TriangleData* data = &mesh->triangles[i];
      for (int t = 0; t < 3; t++) {
        float z = data->points[t].point.z * scaleZ + deco->location.z;
        if (current.z < z) {
          current.z = z;
        }
      }
    }
  }

  return current;
}

glm::vec3 maxPoint(Design* d) {
  glm::vec3 current = glm::vec3(tileSize*.1f, tileSize*.1f, 0);
  if (d->flags & _designAquatic) {
    if (current.x < d->highTide) current.x = d->highTide;
    if (current.x < d->lowTide) current.x = d->lowTide;
  }

  for (int i = 0; i < d->decos.size(); i++) {
    current = maxPoint(current, &d->decos[i]);
  }

  for (int i=0; i < d->structures.size(); i++) {
    Structure* structure = &d->structures[i];

    glm::vec3 size = structure->size;
    float cangle = cos(structure->angle);
    float sangle = sin(structure->angle);
    glm::vec3 right = glm::vec3(-cangle, sangle, 0) * size.x * .5f;
    glm::vec3 along = glm::vec3(sangle, cangle, 0) * size.y;
    glm::vec3 loc = structure->location;

    current = maxPoint(current, loc-right);
    current = maxPoint(current, loc+right);
    current = maxPoint(current, loc-right+along);
    current = maxPoint(current, loc+right+along);

    float top = loc.z + size.z;
    if (current.z < top) current.z = top;
  }

  return current;
}

void renderDeco(Mesh* decoMesh, Deco* deco, bool simple, item ndx,
    item buildingNdx) {
  glm::vec3 loc = deco->location;
  glm::vec3 uright = glm::vec3(0, 1, 0);
  glm::vec3 ualong = glm::vec3(1, 0, 0);
  glm::vec3 up = glm::vec3(0, 0, 1);
  item type = deco->decoType;

  if (type >= numLegacyDecoTypes) {
    DecoType* decoType = getDecoType(deco->decoType);
    item importNdx = decoType->meshImport;
    if (deco->decoType == statueDecoType()) {
      item statueNdx = getStatueForBuilding(buildingNdx, ndx);
      if (statueNdx <= 0 || statueNdx > sizeStatues()) return;
      importNdx = getStatue(statueNdx)->meshImport;
    }

    if (importNdx <= 0) return;

    MeshImport* import = getMeshImport(importNdx);
    if (!(import->flags & _meshImportComplete)) {
      loadMeshImport(decoType->meshImport);
    }

    if (!(decoType->flags & _decoWindAssigned)) {
      decoType->flags |= _decoWindAssigned;
      if (decoType->wind != 0) {
        for (int s = 0; s < 2; s++) {
          item meshNdx = s ? import->mesh : import->simpleMesh;
          if (meshNdx <= 0) continue;
          Mesh* mesh = getMesh(meshNdx);
          //SPDLOG_INFO("tris {}", mesh->numTriangles);
          for (int i = 0; i < mesh->numTriangles; i++) {
            TriangleData* data = &mesh->triangles[i];
            for (int t = 0; t < 3; t++) {
              data->points[t].texture.z = decoType->wind *
                data->points[t].point.z * 2;
            }
          }
        }
      }
    }

    glm::vec3 scale = glm::vec3(1,1,1);
    if (decoType->flags & _decoScaleX) scale.x = deco->scale;
    if (decoType->flags & _decoScaleY) scale.y = deco->scale;
    if (decoType->flags & _decoScaleZ) scale.z = deco->scale;

    if (!simple && import->mesh != 0) {
      insertMesh(decoMesh, import->mesh, loc, deco->yaw, 0, scale);
    } else if (simple && import->simpleMesh != 0) {
      insertMesh(decoMesh, import->simpleMesh, loc, deco->yaw, 0, scale);
    }

  } else if (type == TreeDeco) {
    float sizeMult = ((ndx*163)%256)/1024.f + 1;
    glm::vec3 bellyLoc = loc;
    bellyLoc.z += treeBelly;
    if (simple) {
      makeSimpleTree(decoMesh, bellyLoc,
        treeHeight*sizeMult, treeWidth*sizeMult, colorTree);
    } else {
      makeTree(decoMesh, bellyLoc,
        treeHeight*sizeMult, treeWidth*sizeMult, colorTree, colorBrown);
    }

  } else if (type == ShortSign && !simple) {
    float height = 2;
    float length = 5;
    float width = .5f;
    makeAngledCube(decoMesh,
      glm::vec3(loc.x-width*.5f, loc.y-length*.5f, loc.z-height),
      uright*length, ualong*width, up*height*2.f,
      true, colorBeige);

    height = .7f;
    float descent = 4;
    length = 6;
    width = 1;
    makeAngledCube(decoMesh,
      glm::vec3(loc.x-width*.5f, loc.y-length*.5f, loc.z-descent),
      uright*length, ualong*width, up*(height+descent),
      true, colorBrown);

  } else if (type == ColumnSign && !simple) {
    float height = .5f;
    float length = 2;
    float width = 6;
    makeAngledCube(decoMesh,
      glm::vec3(loc.x-width*.5f, loc.y-length*.5f, loc.z-height),
      uright*length, ualong*width, up*height*3.f,
      true, colorBrown);

    for (int i = -1; i < 2; i += 2) {
      height = 6;
      length = 1;
      width = .5f;
      makeAngledCube(decoMesh,
        glm::vec3(loc.x-width*.5f+i*2.5f, loc.y-length*.5f, loc.z),
        uright*length, ualong*width, up*height,
        true, colorBrown);
    }

    for (int i = 0; i < 4; i++) {
      height = 1;
      length = .5f;
      width = 5;
      makeAngledCube(decoMesh,
        glm::vec3(loc.x-width*.5f, loc.y-length*.5f, loc.z+i*height*1.5+1),
        uright*length, ualong*width, up*height,
        true, colorBeige);
    }

  } else if (type == TallSign) {
    glm::vec3 xdecoDarkWind = glm::vec3(colorBrown.x, colorBrown.y, 1);
    glm::vec3 xdecoDarkWindHalf = glm::vec3(colorBrown.x, colorBrown.y, 0.4);
    glm::vec3 xdecoLightWind = glm::vec3(colorBeige.x, colorBeige.y, 1);
    float height = .5f;
    float length = 1.5f;
    float width = 1.5f;
    if (!simple) {
      makeAngledCube(decoMesh,
        glm::vec3(loc.x-width*.5f, loc.y-length*.5f, loc.z-height),
        uright*length, ualong*width, up*height*3.f,
        true, colorBrown);
    }

    makeCylinder(decoMesh, loc, 5, .375f, 6,
        colorBrown, xdecoDarkWindHalf);
    makeCylinder(decoMesh, loc+up*5.f, 5, .375f, 6,
        xdecoDarkWindHalf, xdecoDarkWind);

    height = .5f;
    length = 1;
    width = 5.5f;
    makeAngledCube(decoMesh,
      glm::vec3(loc.x-width*.5f, loc.y-length*.5f, loc.z+10),
      uright*length, ualong*width, up*height,
      true, xdecoDarkWind);

    height = 3;
    length = .5f;
    width = 5;
    makeAngledCube(decoMesh,
      glm::vec3(loc.x-width*.5f, loc.y-length*.5f, loc.z+10.5f),
      uright*length, ualong*width, up*height,
      true, xdecoLightWind);

    height = .5f;
    length = 1;
    width = 5.5f;
    makeAngledCube(decoMesh,
      glm::vec3(loc.x-width*.5f, loc.y-length*.5f, loc.z+13.5f),
      uright*length, ualong*width, up*height,
      true, xdecoDarkWind);

  } else if (type == Smokestack) {
    int sides = simple?6:24;
    makeCylinder(decoMesh, loc-up, glm::vec3(0, 0, 2), 3, sides, colorBrown);
    makePipe(decoMesh, loc, glm::vec3(0, 0, 25), 2.5f, 2, sides,
        colorBrown);
    makePipe(decoMesh, loc+up*25.f, glm::vec3(0, 0, 1), 3, 2, sides, colorBrown);

  } else if ((type == DecoPool && !simple) || type == DecoBigPool) {
    const bool big = type == DecoBigPool;
    const float xi = big ? 6.f : 2.f;
    const float yi = big ? 10.f : 4.f;
    const float t = -.5f;
    const float hwater = .6f;
    const float hwall = .7f;
    const float depth = 4.f;
    glm::vec3 wall = glm::vec3(0,0,depth+hwall);
    glm::vec3 loc3 = loc;

    makeAngledCube(decoMesh,
      loc3 + glm::vec3(-xi-t, -yi-t, -depth),
      glm::vec3(xi*2+t*2,0,0), glm::vec3(0,t,0), wall,
      true, colorBeige);
    makeAngledCube(decoMesh,
      loc3 + glm::vec3(-xi-t, yi, -depth),
      glm::vec3(xi*2+t*2,0,0), glm::vec3(0,t,0), wall,
      true, colorBeige);
    makeAngledCube(decoMesh,
      loc3 + glm::vec3( xi, -yi, -depth),
      glm::vec3(t,0,0), glm::vec3(0,yi*2,0), wall,
      true, colorBeige);
    makeAngledCube(decoMesh,
      loc3 + glm::vec3(-xi-t, -yi, -depth),
      glm::vec3(t,0,0), glm::vec3(0,yi*2,0), wall,
      true, colorBeige);
    makeQuad(decoMesh,
        loc3+glm::vec3(-xi, -yi, hwater),
        loc3+glm::vec3( xi, -yi, hwater),
        loc3+glm::vec3(-xi,  yi, hwater),
        loc3+glm::vec3( xi,  yi, hwater),
        colorWaterBlue, colorWaterBlue);

  } else if (type == BigTank) {
    int sides = simple?6:24;
    makeCylinder(decoMesh, loc-up, glm::vec3(0, 0, 16), 10, sides, colorBeige);

  } else if (type == SmallTanks && !simple) {
    int sides = simple?6:24;
    for (int i = 0; i < 4; i++) {
      glm::vec3 tloc = loc - up + glm::vec3(i/2-0.5f, i%2-0.5f, 0)*2.f;
      makeCylinder(decoMesh, tloc, glm::vec3(0, 0, 5), 2, sides,
          colorBeige);
      if (i/2 == 0) {
        makeCube(decoMesh, tloc+glm::vec3(1,0,0), glm::vec3(5.f, .5f, 5.5f),
              colorBrown, true, false);
      }
    }

  } else if (type == ShippingContainers && !simple) {
    bool horz = ndx%2 == 1;
    for (int i = 0; i < 6; i++) {
      float y = (i/2 && !(i%2))*.25f + (i%2 ? 1.5f : -1.f);
      float z = 2*(i/2)-2.1f;
      glm::vec3 color = (i == ndx % 5) ? colorTree : colorBrown;
      glm::vec3 offset;
      glm::vec3 size;
      if (horz) {
        offset = glm::vec3(0, y, z);
        size = glm::vec3(6,2,2);
      } else {
        offset = glm::vec3(y, 0, z);
        size = glm::vec3(2,6,2);
      }
      glm::vec3 cloc = loc + offset;
      makeCube(decoMesh, cloc, size, color, true, false);
    }

  } else if (type == HangingSign && !simple) {
    glm::vec3 sloc = loc + glm::vec3(-2, 0, 3);
    makeCube(decoMesh, sloc, glm::vec3(3, .25f, 2), colorBeige, true, false);
    makeCylinder(decoMesh, sloc + glm::vec3(3,0,2), glm::vec3(-4.6f,0,0),
        .25f, 6, colorBrown);

  } else if (type == SwingSet && !simple) {
    float height = 2.5f;
    float width = height;
    float length = 2;
    float thickness = 0.25;
    int numSides = 6;
    glm::vec3 sloc = loc - glm::vec3(0,0,height-.5f);
    glm::vec3 seatloc = loc+glm::vec3(0,0,height*.5f-thickness);
    glm::vec3 xdecoDarkWind = glm::vec3(colorBrown.x, colorBrown.y, 2);

    //Top Beam
    makeCylinder(decoMesh, sloc + glm::vec3(-length,0,height*2-thickness),
        glm::vec3(length*2,0,0), thickness, numSides, colorBrown);

    //Poles
    for (int i=0; i < 4; i++) {
      float x = i%2==0 ? -1 : 1;
      float y = i/2==0 ? -1 : 1;
      makeCylinder(decoMesh, sloc + glm::vec3(length*x,width*y,0),
          glm::vec3(0,-width*y,height*2), thickness, numSides, colorBrown);
    }

    //Swings
    for (int i=0; i < 4; i++) {
      makeCylinder(decoMesh, seatloc + glm::vec3(.75f*(i-1.5f), 0.f, 0.f),
          height-thickness-.5f,
          thickness*.25f, numSides, xdecoDarkWind, colorBrown);
    }
    for (int i=0; i < 2; i++) {
      makeCube(decoMesh, seatloc + glm::vec3(.75f*(i*2-1), 0.f, 0.f),
          glm::vec3(1.f, 0.25f, 0.125f), xdecoDarkWind, true, false);
    }

  } else if (type == VerticalFence) {
    makeCube(decoMesh, loc-up*4.f, glm::vec3(24.125f, .25f, 8),
        colorBrown, true, false);

  } else if (type == HorizontalFence) {
    makeCube(decoMesh, loc-up*4.f, glm::vec3(.25f, 24.125f, 8),
        colorBrown, true, false);

  } else if (type == VerticalFenceLong) {
    makeCube(decoMesh, loc-up*4.f, glm::vec3(72.125f, .25f, 8),
        colorBrown, true, false);

  } else if (type == HorizontalFenceLong) {
    makeCube(decoMesh, loc-up*4.f, glm::vec3(.25f, 72.125f, 8),
        colorBrown, true, false);

  } else if (type == ShrubsV) {
    makeFlaredCube(decoMesh, loc-up*4.f, glm::vec3(12.f, 2.f, 6.f), 2, colorTree,
        glm::vec3(colorTree.x, colorTree.y, 1));

  } else if (type == ShrubsH) {
    makeFlaredCube(decoMesh, loc-up*4.f, glm::vec3(2.f, 12.f, 6.f), 2, colorTree,
        glm::vec3(colorTree.x, colorTree.y, 1));

  } else if (type == PathV) {
    makeFlaredCube(decoMesh, loc-up*2.f, glm::vec3(12.f, 4.f, 2.5f), paveFlare,
        colorBeige, colorBeige);

  } else if (type == PathH) {
    makeFlaredCube(decoMesh, loc-up*2.f, glm::vec3(4.f, 12.f, 2.5f), paveFlare,
        colorBeige, colorBeige);

  } else if (type == Pavilion) {
    makeFlaredCube(decoMesh, loc-up*2.f, glm::vec3(12.f, 12.f, 2.5f), paveFlare,
        colorBeige, colorBeige);

  } else if (type == RoadV) {
    makeFlaredCube(decoMesh, loc-up*2.f, glm::vec3(12.f, 4.f, 2.4f), paveFlare,
        colorPavement, colorPavement);

  } else if (type == RoadH) {
    makeFlaredCube(decoMesh, loc-up*2.f, glm::vec3(4.f, 12.f, 2.4f), paveFlare,
        colorPavement, colorPavement);

  } else if (type == Parking) {
    makeFlaredCube(decoMesh, loc-up*2.f, glm::vec3(12.f, 12.f, 2.4f), paveFlare,
        colorPavement, colorPavement);

  } else if (type == ParkingSuper) {
    makeFlaredCube(decoMesh, loc-up*2.f, glm::vec3(36.f, 36.f, 2.4f), paveFlare,
        colorPavement, colorPavement);

  } else if (type == VerticalCropsGreen ||
        type == HorizontalCropsGreen ||
        type == VerticalCropsYellow ||
        type == HorizontalCropsYellow) {
    bool isVertical = type == VerticalCropsGreen ||
      type == VerticalCropsYellow;
    float rowWidth = 4.f;
    float xs = isVertical ? 75.f : simple ? 0.f : rowWidth;
    float ys = !isVertical ? 75.f : simple ? 0.f : rowWidth;
    float zs = 6.f;
    glm::vec3 offset = isVertical ?  glm::vec3(0, rowWidth*2, 0) :
      glm::vec3(rowWidth*2, 0, 0);
    glm::vec3 tex = type == VerticalCropsGreen || type == HorizontalCropsGreen ?
      colorLightGreen : colorYellowFarm;
    glm::vec3 texTop = glm::vec3(tex.x, tex.y, 0);
    int count = tileSize/rowWidth;
    if (simple) {
      makeFlaredCube(decoMesh, loc-up*4.f,
          glm::vec3(xs, ys, zs)+offset*float(count), 3, tex, tex);
    } else {
      for (int i = 0; i < count; i++) {
        makeFlaredCube(decoMesh, loc-up*4.f + float(i-count/2)*offset,
            glm::vec3(xs, ys, zs), 3, tex, texTop);
      }
    }

  } else if (type == Awning && !simple) {
    int sides = 6;
    glm::vec3 xdecoDarkWind = glm::vec3(colorBrown.x, colorBrown.y, 0.2);
    for (int i = 0; i < 4; i++) {
      glm::vec3 ploc = loc-up*3.f + glm::vec3((i/2-0.5f)*5.75f, (i%2-0.5f)*7.75f, 0);
      makeCylinder(decoMesh, ploc, 6, .125f, sides, colorBrown, xdecoDarkWind);
    }
    makeFlaredCube(decoMesh, loc+up*3.f, glm::vec3(2.25f, 4.25f, 1.5f), 2,
        glm::vec3(colorRed.x, colorRed.y, 0.2),
        glm::vec3(colorRed.x, colorRed.y, 1));

  } else if (type >= BleachersN && type <= BleachersW) {
    glm::vec3 along =
      type == BleachersN ? glm::vec3(0, 1, 0) :
      type == BleachersE ? glm::vec3( 1,0, 0) :
      type == BleachersS ? glm::vec3(0,-1, 0) :
                           glm::vec3(-1,0, 0);
    glm::vec3 right = glm::vec3(along.y, -along.x, 0);
    int numSteps = 16;
    float blechWidth = 20.f;
    glm::vec3 ploc = loc - up*3.25f - (numSteps*.5f)*right -
      along*blechWidth*.5f;

    for (int s = 0; s < numSteps; s++) {
      makeAngledCube(decoMesh, ploc + right*(1.f*s),
          along*blechWidth, right*.75f, up*(.5f*s), true, colorTree);
      makeAngledCube(decoMesh, ploc + right*(1.f*s+.75f),
          along*blechWidth, right*.25f, up*(.5f*(s + 2)), true, colorTree);
    }

  } else if (type == BaseballDiamond) {
    glm::vec3 along = glm::vec3(0,1,0);
    glm::vec3 right = glm::vec3(1,0,0);
    glm::vec3 ploc = loc - up*3.f;
    glm::vec3 plocUp = loc + up*.25f;
    glm::vec3 tx = glm::vec3(colorYellow.x, colorYellow.y, 0.f);
    float dbSize = 20.f;
    glm::vec3 home = ploc - along*10.f - right*10.f;
    glm::vec3 homeUp = plocUp - along*10.f - right*10.f;
    glm::vec3 homeTop = loc + up*.3f - along*10.f - right*10.f;
    float mf = .5f;
    float dbFlare = dbSize*2.f + 4.f;

    // Sides and Lines
    makeFlaredCube(decoMesh, ploc - right * 12.5f + along*dbSize*.5f,
        glm::vec3(5.f, dbSize*2.f, 3.3f), 4.f, tx, tx);
    makeFlaredCube(decoMesh, ploc - along * 12.5f + right*(dbSize*.5f - 2.5f),
        glm::vec3(dbSize*2.f+5.f, 5.f, 3.3f), 4.f, tx, tx);

    makeFlaredCube(decoMesh, plocUp - right * 10.f + along*dbSize*.375f,
        glm::vec3(.25f, dbSize*1.5f, .26f), mf, colorBeige, colorBeige);
    makeFlaredCube(decoMesh, plocUp - along * 10.f + right* dbSize*.375f,
        glm::vec3(dbSize*1.5f, .25f, .26f), mf, colorBeige, colorBeige);

    // Mounds
    makeFlaredCube(decoMesh, plocUp, glm::vec3(1.f, 1.f, .5f), mf,
        colorBeige, colorBeige);
    makeFlaredCube(decoMesh, homeUp, glm::vec3(1.f, 1.f, .5f), mf,
        colorBeige, colorBeige);
    makeFlaredCube(decoMesh, homeUp+along*dbSize, glm::vec3(1.f, 1.f, .5f), mf,
        colorBeige, colorBeige);
    makeFlaredCube(decoMesh, homeUp+right*dbSize, glm::vec3(1.f, 1.f, .5f), mf,
        colorBeige, colorBeige);
    makeFlaredCube(decoMesh, homeUp+right*dbSize+along*dbSize,
        glm::vec3(1.f, 1.f, .5f), mf, colorBeige, colorBeige);

    for (int i = 0; i < 12; i++) {
      int j = i+1;
      float theta0 = i/12.f*pi_o*.5f;
      float theta1 = j/12.f*pi_o*.5f;
      glm::vec3 udir0 = glm::vec3(cos(theta0), sin(theta0), 0);
      glm::vec3 udir1 = glm::vec3(cos(theta1), sin(theta1), 0);

      glm::vec3 p0 = homeTop + udir0*dbSize*2.f;
      glm::vec3 p1 = homeTop + udir1*dbSize*2.f;
      glm::vec3 p2 = home + udir0*dbFlare;
      glm::vec3 p3 = home + udir1*dbFlare;

      Triangle t1 = {{
        {p0, up, tx},
        {p1, up, tx},
        {homeTop, up, tx},
      }};
      pushTriangle(decoMesh, t1);
      makeQuad(decoMesh, p1, p0, p3, p2,
          up, up, p3-home, p2-home, tx, tx);
    }

  } else if (type == Facade) {
    glm::vec3 along = glm::vec3(1,0,0);
    glm::vec3 right = glm::vec3(0,1,0);
    int numSteps = 20;
    int numColumns = 8;
    float baseHeight = 7.f;
    float baseWidth = numColumns*2+.5f;
    float baseDepth = 5.f;
    float stepWidth = .5f;
    float stepHeight = baseHeight / numSteps;
    float colHeight = 6.f;
    glm::vec3 ploc = loc-up*3.f + .25f*along*(baseDepth+numSteps*stepWidth);

    for (int s = 0; s < numSteps; s++) {
      makeCube(decoMesh, ploc - along*.5f*(baseDepth+s),
          glm::vec3(stepWidth, baseWidth, stepHeight*(numSteps-s-1)),
          colorBeige, true, false);
    }

    makeCube(decoMesh, ploc, glm::vec3(baseDepth, baseWidth, baseHeight),
        colorBeige, true, false);

    for (int c = 0; c < numColumns; c++) {
      glm::vec3 col = ploc + up*baseHeight + right*(c*2.f-numColumns+1) +
        along*.5f*(baseDepth-2);
      makeCube(decoMesh, col, glm::vec3(1.f, 1.f, .25f), colorBeige, true, false);
      makeCylinder(decoMesh, col, colHeight, .375f, simple ? 6 : 24,
          colorBeige);
      makeCube(decoMesh, col+up*colHeight, glm::vec3(1.f, 1.f, .25f),
          colorBeige, false, false);
    }

    makeCube(decoMesh, ploc + up*(baseHeight+colHeight) +
        along*.5f*(baseDepth-2), glm::vec3(2.f, baseWidth, 1.f),
        colorBeige, true, false);
  }
}

Box boxForStructure(Structure* s) {
  float cangle = cos(s->angle);
  float sangle = sin(s->angle);
  //glm::vec3 along = glm::vec3(sangle, cangle, 0) * s->size.y;
  //return box(s->location, glm::vec2(s->size), along);
  glm::vec3 size = s->size;
  glm::vec2 normnorm = normalize(glm::vec2(sangle, cangle));
  glm::vec2 axis0 = normnorm * size.y;
  glm::vec2 axis1 = glm::vec2(normnorm.y, -normnorm.x) * size.x;
  glm::vec2 corner = glm::vec2(s->location) - axis1*.5f;
  return box(corner, axis0, axis1);
}

bool structureCollide(Design* design, int i) {
  Box b0 = boxForStructure(&design->structures[i]);
  for (int j = 0; j < design->structures.size(); j++) {
    if (i == j) continue;
    Box b1 = boxForStructure(&design->structures[j]);
    if (boxIntersect(b0, b1)) {
      return true;
    }
  }
  return false;
}

void renderDesign(item designNdx, item buildingNdx) {
  Design* design = getDesign(designNdx);
  if (design->mesh == 0) {
    design->mesh = addMesh();
    design->simpleMesh = addMesh();
    design->decoMesh = addMesh();
    design->simpleDecoMesh = addMesh();
  }

  Mesh* mesh = getMesh(design->mesh);
  Mesh* simpleMesh = getMesh(design->simpleMesh);
  Mesh* decoMesh = getMesh(design->decoMesh);
  Mesh* simpleDecoMesh = getMesh(design->simpleDecoMesh);
  buildingRenderSeed = designNdx;

  for (int i=0; i < design->structures.size(); i++) {
    if (!isStructuresVisible() &&
        (getSelectionType() != SelectionStructure || i != getSelection())) {
      continue;
    }

    renderStructure(mesh, &design->structures[i], design->zone, false);
    renderStructure(simpleMesh, &design->structures[i], design->zone, true);

    /*
    glm::vec3 color = structureCollide(design, i) ? colorRed : colorTree;
    Box b = boxForStructure(&design->structures[i]);
    glm::vec3 loc = glm::vec3(b.corner, 20);
    glm::vec3 along = glm::vec3(b.axis0, 0);
    glm::vec3 back = glm::vec3(b.axis1, 0);
    makeQuad(decoMesh, loc+along, loc, loc+along+back, loc+back, color, color);
    */
  }

  for (int i=0; i < design->decos.size(); i++) {
    Deco* deco = &design->decos[i];
    if (!isDecoVisible(deco->decoType) &&
        (getSelectionType() != SelectionDeco || i != getSelection())) {
      continue;
    }

    item type = deco->decoType;
    bool buildingTex = type >= numLegacyDecoTypes ?
      getDecoType(type)->flags & _decoUseBuildingTexture :
      false;
    renderDeco(buildingTex ? mesh : decoMesh, deco, false, i, buildingNdx);
    renderDeco(buildingTex ? simpleMesh : simpleDecoMesh, deco, true, i, buildingNdx);
  }

  if (decoCull.size() <= designNdx+20) {
    decoCull.resize(designNdx+20);
  }
  decoCull[designNdx] = 1000;

  for (int i=0; i < design->decos.size(); i++) {
    switch(design->decos[i].decoType) {
      case TreeDeco:              {setDecoCull( 500, designNdx);} break;
      case VerticalFence:         {setDecoCull( 900, designNdx);} break;
      case HorizontalFence:       {setDecoCull( 900, designNdx);} break;
      case ShrubsV:               {setDecoCull( 400, designNdx);} break;
      case ShrubsH:               {setDecoCull( 400, designNdx);} break;
      case PathV:                 {setDecoCull( 100, designNdx);} break;
      case PathH:                 {setDecoCull( 100, designNdx);} break;
      case RoadV:                 {setDecoCull( 100, designNdx);} break;
      case RoadH:                 {setDecoCull( 100, designNdx);} break;
      case SwingSet:              {setDecoCull(  50, designNdx);} break;
      case DecoPool:              {setDecoCull( 200, designNdx);} break;
      case DecoBigPool:           {setDecoCull( 500, designNdx);} break;
      case ShortSign:             {setDecoCull(  50, designNdx);} break;
      case ColumnSign:            {setDecoCull( 100, designNdx);} break;
      case TallSign:              {setDecoCull( 200, designNdx);} break;
      case HangingSign:           {setDecoCull(  50, designNdx);} break;
      case Awning:                {setDecoCull( 100, designNdx);} break;
      case Smokestack:            {setDecoCull( 200, designNdx);} break;
      case ShippingContainers:    {setDecoCull(  50, designNdx);} break;
      case SmallTanks:            {setDecoCull(  50, designNdx);} break;
      case BigTank:               {setDecoCull( 100, designNdx);} break;
      case VerticalCropsGreen:    {setDecoCull(1000, designNdx);} break;
      case HorizontalCropsGreen:  {setDecoCull(1000, designNdx);} break;
      case VerticalCropsYellow:   {setDecoCull(1000, designNdx);} break;
      case HorizontalCropsYellow: {setDecoCull(1000, designNdx);} break;
      case BleachersN:            {setDecoCull( 500, designNdx);} break;
      case BleachersE:            {setDecoCull( 500, designNdx);} break;
      case BleachersS:            {setDecoCull( 500, designNdx);} break;
      case BleachersW:            {setDecoCull( 500, designNdx);} break;
      case BaseballDiamond:       {setDecoCull(1000, designNdx);} break;
      case Facade:                {setDecoCull( 200, designNdx);} break;
      case Pavilion:              {setDecoCull( 100, designNdx);} break;
      case Parking:               {setDecoCull( 200, designNdx);} break;
      case ParkingSuper:          {setDecoCull( 400, designNdx);} break;
      case VerticalFenceLong:     {setDecoCull(1200, designNdx);} break;
      case HorizontalFenceLong:   {setDecoCull(1200, designNdx);} break;
    }
  }

  // Fallback elevator
  if (c(CShowFallbackElevators)) {
    glm::vec3 center = glm::vec3(1.f, -.5f*design->size.y, 0);
    glm::vec3 diag = glm::vec3(design->size.x*.5f, design->size.y*.5f, 0);
    glm::vec3 pad = glm::vec3(2, 4, 0);
    glm::vec3 up = glm::vec3(0,0,20);
    glm::vec3 down = glm::vec3(0,0,2);
    makeFlaredCube(decoMesh, center+diag-up-down, 2.f*diag+up+pad,
        4, colorDarkSand, colorDarkSand, true);
    //makeFlaredCube(simpleDecoMesh, center+diag-up-down, 2.f*diag+up+pad,
     //   20, colorDarkSand, colorDarkSand);
  }

  bufferMesh(design->mesh);
  bufferMesh(design->decoMesh);
  bufferMesh(design->simpleMesh);
  bufferMesh(design->simpleDecoMesh);

  resizeDesign(designNdx);
  setMaxLandValues(designNdx);
}

void renderDesign(item designNdx) {
  renderDesign(designNdx, 0);
}

item getIssueIconMesh(item ndx) {
  if (issueIconMeshes.size() <= ndx) issueIconMeshes.resize(ndx+1);
  if (issueIconMeshes[ndx] != 0) return issueIconMeshes[ndx];

  glm::vec3 ico;
  switch(ndx) {
    case Jobless:      {ico = iconWorker;} break;
    case Hungry:       {ico = iconFood;} break;
    case Homeless:     {ico = iconHouse;} break;
    case NeedsWorkers: {ico = iconHelpWanted;} break;
    case NoCustomers:  {ico = iconTrade;} break;
    case NeedsFreight: {ico = iconTruck;} break;
    case SickIssue:    {ico = iconSick;} break;
    case NoIssue:
    default:           {ico = iconNull;} break;
  }

  item meshNdx = addMesh();
  Mesh* mesh = getMesh(meshNdx);
  float size = tileSize;
  glm::vec3 right = glm::vec3(size, 0, 0);
  glm::vec3 down = glm::vec3(0, size, 0);
  glm::vec3 topLeft = -.5f*(right+down) + glm::vec3(0,0,20);
  Line icoLine = iconToSpritesheet(ico);
  makeQuad(mesh, topLeft, topLeft+right,
      topLeft+down, topLeft+right+down, icoLine.start, icoLine.end);
  bufferMesh(meshNdx);

  if (issueIconMeshes.size() <= ndx) issueIconMeshes.resize(ndx+1);
  issueIconMeshes[ndx] = meshNdx;
  return meshNdx;
};

void renderBuilding(item ndx) {
  Building* building = getBuilding(ndx);
  if (!(building->flags & _buildingExists)) {
    return;
  }

  if (building->entity == 0) {
    building->entity = addEntity(BuildingShader);
    building->decoEntity = addEntity(DecoShader);
  }

  Design* design = getDesign(building->design);

  float cullSize = design->size.x*.5f + design->size.y +
    design->size.z*5;
  float cullDist = design->size.z * c(CBuildingCullZFactor)
    + (design->size.x*.5f + design->size.y) * c(CBuildingCullSizeFactor)
    + c(CBuildingCullMin);
  if (building->flags & _buildingCity) cullDist *= 4;

  Entity* entity = getEntity(building->entity);
  entity->texture = buildingTexture;
  bool illum = getGameMode() == ModeBuildingDesigner ||
      (building->flags & _buildingLights);
  setEntityIlluminated(building->entity, illum);
  setCull(building->entity, cullSize, cullDist);
  entity->simpleDistance = cullDist*c(CBuildingCullSimplify);

  Entity* decoEntity = getEntity(building->decoEntity);
  decoEntity->texture = paletteTexture;
  decoEntity->flags |= _entityWind;
  setCull(building->decoEntity, cullSize, cullDist*c(CBuildingCullDeco));
  decoEntity->simpleDistance = cullDist * c(CBuildingCullSimplifyDeco);
  //setCull(building->decoEntity, cullSize, decoCull[building->design]);
  //decoEntity->simpleDistance = decoCull[building->design] * .5f;
  setEntityIlluminated(building->decoEntity, illum);

  if (design->flags & _designHasStatue) {
    // Force statue designs to re-render for this building
    design->mesh = entity->mesh;
    design->simpleMesh = entity->simpleMesh;
    design->decoMesh = decoEntity->mesh;
    design->simpleDecoMesh = decoEntity->simpleMesh;
    renderDesign(building->design, ndx);

  } else if (design->mesh == 0) {
    renderDesign(building->design, ndx);
  }

  entity->mesh = design->mesh;
  entity->simpleMesh = design->simpleMesh;
  decoEntity->mesh = design->decoMesh;
  decoEntity->simpleMesh = design->simpleDecoMesh;

  float angle = atan2(building->normal.y, building->normal.x);
  placeEntity(building->entity, building->location, angle, 0.f);
  placeEntity(building->decoEntity, building->location, angle, 0.f);

  paintBuilding(ndx);
}

void paintBuilding(item ndx) {
  Building* building = getBuilding(ndx);
  if (!(building->flags & _buildingExists)) {
    return;
  }

  if (building->entity == 0) {
    renderBuilding(ndx);
  }

  assignBuildingTexture(ndx);
  Entity* entity = getEntity(building->entity);
  Entity* decoEntity = getEntity(building->decoEntity);

  bool abandoned = building->flags & _buildingAbandoned;
  if (abandoned) {
    entity->flags |= _entityHeatmapLimited;
    decoEntity->flags |= _entityHeatmapLimited;
  } else {
    entity->flags &= ~_entityHeatmapLimited;
    decoEntity->flags &= ~_entityHeatmapLimited;
  }

  bool viz = building->zone == GovernmentZone || buildingsVisible() ||
    building->flags & _buildingHistorical;
  setEntityVisible(building->entity, viz);
  setEntityVisible(building->decoEntity, viz);

  if (building->flags & _buildingComplete) {
    entity->flags &= ~_entityTransparent;
    decoEntity->flags &= ~_entityTransparent;
  } else {
    entity->flags |= _entityTransparent;
    decoEntity->flags |= _entityTransparent;
    if (building->plan != 0) {
      Plan* plan = getPlan(building->plan);
      bool legal = plan->legalMessage != 0;
      setEntityRedHighlight(building->entity, legal);
      setEntityRedHighlight(building->decoEntity, legal);
    }
  }

  IssueIcon issue = c(CShowIssueIcons) ? getBuildingIssue(ndx) : NoIssue;
  if (issue == NoIssue) {
    if (building->iconEntity != 0) {
      removeEntity(building->iconEntity);
      building->iconEntity = 0;
    }

  } else {
    if (building->iconEntity == 0) {
      building->iconEntity = addEntity(WSUIInstancedShader);
    }

    Entity* iconEntity = getEntity(building->iconEntity);
    iconEntity->texture = iconTexture;
    iconEntity->mesh = getIssueIconMesh(issue);
    setEntityRedHighlight(building->iconEntity, true);
    setEntityVisible(building->iconEntity, issuesIconsVisible());
    setEntityTransparent(building->iconEntity, false);
    glm::vec3 loc = getBuildingTop(ndx) + glm::vec3(0,0,25);
    placeEntity(building->iconEntity, loc, 0.f, 0.f);
    setCull(building->iconEntity, 100, 5000);
  }
}

static const glm::vec3 handleSize = glm::vec3(3, 3, 3);
static const glm::vec3 up = glm::vec3(0, 0, handleSize.z);

void renderBoundaries(Mesh* mesh, Mesh* textMesh) {
  if (c(CObjectViewer)) return;
  Design* design = getDesign(1);
  glm::vec3 uright = glm::vec3(1, 0, 0);
  glm::vec3 ualong = glm::vec3(0, 1, 0);
  glm::vec3 loc;
  float fontSize = (design->size.x + design->size.y)/tileSize+10;
  float textSetback = 10;
  float textShift = 10;
  char* sizeStrX = sprintf_o("%.1f", ceil(design->size.x/tileSize*10)*.1f);
  char* sizeStrY = sprintf_o("%.1f", ceil(design->size.y/tileSize*10)*.1f);

  for (float i=-0.5; i < 1; i+=1) {
    loc = glm::vec3(.1f, design->size.y*i+.5f, -.5f);
    makeAngledCube(mesh, loc, uright*(design->size.x-.1f), -ualong,
      up*.5f, true, colorRed);
  }

  loc = glm::vec3(design->size.x, design->size.y*.5f + .5f, -.5f);
  makeAngledCube(mesh, loc, uright, -ualong*(design->size.y+1),
    up*.5f, true, colorRed);

  renderString(textMesh, sizeStrX,
    glm::vec3(textShift, design->size.y*.5f+textSetback, 5),
    uright*fontSize);
  renderString(textMesh, sizeStrX,
    glm::vec3(design->size.x-textShift, -design->size.y*.5f-textSetback, 5),
    -uright*fontSize);

  renderString(textMesh, sizeStrY,
    glm::vec3(-textSetback, -design->size.y*.5f+textShift, 5),
    ualong*fontSize, -uright*fontSize);
  renderString(textMesh, sizeStrY,
    glm::vec3(design->size.x+textSetback, design->size.y*.5f-textShift, 5),
    -ualong*fontSize, uright*fontSize);

  free(sizeStrX);
  free(sizeStrY);

  if (isDesignAquatic(1)) {
    item decoNdx = getDecoTypeByName("DecoBuoy1");
    DecoType* deco = getDecoType(decoNdx);
    MeshImport* import = getMeshImport(deco->meshImport);
    if (!(import->flags & _meshImportComplete)) {
      loadMeshImport(deco->meshImport);
    }

    float mapSize = getMapSize();
    float spacing = mapSize/100;
    float lowTide = getDesign(1)->lowTide;
    for (float i = spacing*.5f; i < mapSize; i += spacing) {
      glm::vec3 loc = glm::vec3(lowTide, i-mapSize*.5f, 0);
      insertMesh(mesh, import->mesh, loc, 0, 0, glm::vec3(1,1,1));
    }
  }
}

void setupHandles() {
  float angle = 0;
  glm::vec3 location(0,0,c(CSuperflatHeight)+100);
  if (getGameMode() == ModeBuildingDesigner) {
    Building* building = getBuilding(1);
    location = building->location;
    angle = atan2(building->normal.y, building->normal.x);
  } else {
    location.x = getChunkSize()*tileSize*5;
    location.y = getChunkSize()*tileSize*5;
  }

  Entity* entity = getEntity(handlesEntity);
  entity->texture = paletteTexture;
  setEntityBringToFront(handlesEntity, true);
  setEntityHighlight(handlesEntity, true);
  placeEntity(handlesEntity, location, angle, 0.f);

  Entity* textEntity = getEntity(designTextEntity);
  textEntity->texture = textTexture;
  setEntityRedHighlight(designTextEntity, true);
  setEntityHighlight(designTextEntity, true);
  placeEntity(designTextEntity, location, angle, 0.f);
  setCull(designTextEntity, 1000000, 10000000);

  Entity* bEntity = getEntity(bordersEntity);
  bEntity->texture = paletteTexture;
  setEntityHighlight(bordersEntity, true);
  placeEntity(bordersEntity, location, angle, 0.f);

  createMeshForEntity(handlesEntity);
  createMeshForEntity(designTextEntity);
  createMeshForEntity(bordersEntity);
  Mesh* mesh = getMeshForEntity(handlesEntity);
  Mesh* textMesh = getMeshForEntity(designTextEntity);
  Mesh* bordersMesh = getMeshForEntity(bordersEntity);

  if (getGameMode() == ModeBuildingDesigner) {
    renderBoundaries(bordersMesh, textMesh);
    bufferMesh(entity->mesh);
    bufferMesh(textEntity->mesh);
    bufferMesh(bEntity->mesh);
  }
}

void renderHandles() {
  Building* building = getBuilding(1);
  Design* design = getDesign(building->design);
  Entity* entity = getEntity(handlesEntity);

  createMeshForEntity(handlesEntity);
  createMeshForEntity(designTextEntity);
  Mesh* mesh = getMeshForEntity(handlesEntity);
  Mesh* textMesh = getMeshForEntity(designTextEntity);

  for (int i=design->structures.size()-1; i >= 0; i--) {
    if (!isStructuresVisible() &&
        (getSelectionType() != SelectionStructure || i != getSelection())) {
      continue;
    }

    Structure* structure = &design->structures[i];

    glm::vec3 size = structure->size;
    float cangle = cos(structure->angle);
    float sangle = sin(structure->angle);
    glm::vec3 right = glm::vec3(-cangle, sangle, 0) * size.x * .5f;
    glm::vec3 along = glm::vec3(sangle, cangle, 0) * size.y;
    glm::vec3 uright = normalize(right)*handleSize.x;
    glm::vec3 ualong = normalize(along)*handleSize.y;
    glm::vec3 loc = structure->location - uright*.5f - ualong*.5f - up*.5f;

    bool gray = getSelectionType() == SelectionStructure &&
      i != getSelection();
    glm::vec3 cRed = gray ? colorDarkGray : colorRed;
    glm::vec3 cGreen = gray ? colorDarkGray : colorBrightGreen;
    glm::vec3 cBlue = gray ? colorDarkGray : colorBlue;

    makeAngledCube(mesh, loc, uright, ualong, up, true, colorRed);

    makeAngledCube(mesh, loc+along, uright, ualong, up, true, colorBlue);
    makeAngledCube(mesh, loc+uright*.375f+ualong,
        uright*.25f, along-ualong, up*.25f, true, cBlue);

    makeAngledCube(mesh, loc+right+glm::vec3(0,0,size.z),
      uright, ualong, up, true, colorBrightGreen);
    makeAngledCube(mesh, loc+up+uright*.375f+ualong*.375f,
      uright*.25f, ualong*.25f, glm::vec3(0,0,size.z-up.z),
      false, cGreen);
    makeAngledCube(mesh, loc+ualong*.375f+uright*.375f+glm::vec3(0,0,size.z),
      right-uright*.25f, ualong*.25f, up*.25f, true, cGreen);

    if (size.z >= 10*sectionLength) {
      int floors = int(round(size.z/sectionLength));
      int startFloor = int(round(structure->location.z/sectionLength));
      char* heightStr = startFloor == 0 ?
        sprintf_o("%d", floors) :
        sprintf_o("%d - %d", startFloor, startFloor+floors);
      glm::vec3 topLoc = structure->location +
        glm::vec3(0,0,size.z - sectionLength);
      float fontSize = 10.f;
      glm::vec3 ualong = normalize(along) * fontSize;
      glm::vec3 uright = normalize(right) * fontSize;
      glm::vec3 hdown = glm::vec3(0,0,-fontSize);
      renderStringCentered(textMesh, heightStr,
          topLoc-right*1.1f+along*.5f,
          -ualong, hdown);
      renderStringCentered(textMesh, heightStr,
          topLoc+right*1.1f+along*.5f,
          ualong, hdown);
      renderStringCentered(textMesh, heightStr,
          topLoc+right*0.f-along*.1f,
          uright, hdown);
      renderStringCentered(textMesh, heightStr,
          topLoc+right*0.f+along*1.1f,
          -uright, hdown);
      free(heightStr);
    }
  }

  setupHandles();
}

void renderDecoHandles() {
  Building* building = getBuilding(1);
  Design* design = getDesign(building->design);
  Entity* entity = getEntity(handlesEntity);
  createMeshForEntity(handlesEntity);
  Mesh* mesh = getMeshForEntity(handlesEntity);

  for (int i=design->decos.size()-1; i >= 0; i--) {
    Deco* deco = &design->decos[i];
    if (!isDecoVisible(deco->decoType) &&
        (getSelectionType() != SelectionDeco || i != getSelection())) {
      continue;
    }

    float cangle = cos(deco->yaw);
    float sangle = sin(deco->yaw);
    glm::vec3 ualong = glm::vec3(cangle, sangle, 0);
    glm::vec3 uright = glm::vec3(-sangle, cangle, 0);
    glm::vec3 up = glm::vec3(0, 0, 1);
    glm::vec3 loc = deco->location - uright*.5f - ualong*.5f - up*.5f;

    bool gray = getSelectionType() == SelectionDeco &&
      i != getSelection();
    glm::vec3 cRed = gray ? colorDarkGray : colorRed;
    glm::vec3 cGreen = gray ? colorDarkGray : colorBrightGreen;

    makeAngledCube(mesh, loc, uright, ualong, up, true, colorRed);

    if (deco->decoType >= numLegacyDecoTypes) {
      glm::vec3 along = ualong * deco->scale * 10.f;
      makeAngledCube(mesh, loc-along,
          uright, ualong, up, true, colorBrightGreen);
      makeAngledCube(mesh, loc+uright*.625f,
          -uright*.25f, ualong-along, up*.25f, true, cGreen);
    }
  }

  setupHandles();
}

void unrenderHandles() {
  setupHandles();
}

void setDesignRender(bool val) {
  if (getGameMode() == ModeGame) return;
  val = true;
  if (handlesEntity != 0) {
    setEntityVisible(handlesEntity, val);
    setEntityVisible(bordersEntity, val);
    setEntityVisible(designTextEntity, val);
  }
}

void designOrganizerRender() {
  if (getGameMode() == ModeGame) return;
  if (handlesEntity == 0) {
    handlesEntity = addEntity(PaletteShader);
    bordersEntity = addEntity(PaletteShader);
    designTextEntity = addEntity(TextShader);
  }

  setupHandles();

  //if (c(CObjectViewer)) return;
  Mesh* textMesh = getMeshForEntity(designTextEntity);
  glm::vec3 uright = glm::vec3(1, 0, 0);
  glm::vec3 ualong = glm::vec3(0, 1, 0);
  float spacing = getChunkSize()*tileSize;
  float fontSize = tileSize*5;
  float z = 10;
  float step = spacing*11;

  for (int i = 0; i <= 10; i++) {
    char* digitStr = sprintf_o("%d", i);
    float w = stringWidth(digitStr)*fontSize*.5f;
    float u0 = spacing*(i+.5f)-w;
    float u1 = spacing*(i+.5f)+w;
    float v = -fontSize;
    renderString(textMesh, digitStr,
      glm::vec3(u1,v,z), -uright*fontSize);
    renderString(textMesh, digitStr,
      glm::vec3(v,u0,z), ualong*fontSize);
    renderString(textMesh, digitStr,
      glm::vec3(u0,step-v,z), uright*fontSize);
    renderString(textMesh, digitStr,
      glm::vec3(step-v,u1,z), -ualong*fontSize);
    free(digitStr);
  }

  float dw = stringWidth("Density")*fontSize*.5f;
  float vw = stringWidth("Value")*fontSize*.5f;
  renderString(textMesh, "Density",
    glm::vec3(-fontSize*3,spacing-dw,z), ualong*fontSize);
  renderString(textMesh, "Value",
    glm::vec3(spacing+vw,-fontSize*3,z), -uright*fontSize);
  renderString(textMesh, "Density",
    glm::vec3(step+fontSize*3,step-spacing+dw,z), -ualong*fontSize);
  renderString(textMesh, "Value",
    glm::vec3(step-spacing-vw,step+fontSize*3,z), uright*fontSize);

  bufferMesh(getEntity(designTextEntity)->mesh);
  setDesignRender(true);
}

void designRender() {
  if (getGameMode() == ModeDesignOrganizer) designOrganizerRender();
  if (getGameMode() == ModeGame) return;
  if (sizeDesigns() <= 0) return;

  Design* d = getDesign(1);
  Building* b = getBuilding(1);
  bool aquatic = isDesignAquatic(1);

  if (wasAquatic != aquatic ||
      d->highTide != lastHighTide || d->lowTide != lastLowTide) {
    if (isDesignAquatic(1)) {
      generateBuildingDesignerLand(b->location.x, d->lowTide, d->highTide);
      b->location.z = c(CAquaticBuildingHeight);
    } else {
      generateBuildingDesignerLand(0, getMapSize(), getMapSize());
      b->location = pointOnLand(b->location);
      //b->location.z = c(CSuperflatHeight) + 1;
    }

    removeBuildingElevator(1);
    addBuildingElevator(1, true);

    lastLowTide = d->lowTide;
    lastHighTide = d->highTide;
    wasAquatic = aquatic;
  }

  if (handlesEntity == 0) {
    handlesEntity = addEntity(PaletteShader);
    bordersEntity = addEntity(PaletteShader);
    designTextEntity = addEntity(TextShader);
  }
  resizeDesign(1);
  renderDesign(1, 0);
  renderBuilding(1);
  paintBuilding(1);
  updateDesignDimensionStrings();

  if (getCurrentTool() == 2) {
    renderHandles();
  } else if (getCurrentTool() == 3) {
    renderDecoHandles();
  } else {
    unrenderHandles();
  }

  setDesignRender(true);
}

void resetDesignRender() {
  handlesEntity = 0;
  bordersEntity = 0;
  designTextEntity = 0;
  lastLowTide = -1;
  lastHighTide = -1;
  wasAquatic = false;
}

void setBuildingHighlight(item ndx, bool highlight) {
  Building* building = getBuilding(ndx);
  if (building == 0) return;
  if (building->entity != 0) {
    setEntityHighlight(building->entity, highlight);
  }
  if (building->decoEntity != 0) {
    setEntityHighlight(building->decoEntity, highlight);
  }
}

void setBuildingRedHighlight(item ndx, bool highlight) {
  Building* building = getBuilding(ndx);
  if (building == 0) return;
  if (building->entity != 0) {
    setEntityRedHighlight(building->entity, highlight);
  }
  if (building->decoEntity != 0) {
    setEntityRedHighlight(building->decoEntity, highlight);
  }
}

