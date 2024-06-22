#pragma once

#include "../line.hpp"
#include "../item.hpp"
#include "../inputEvent.hpp"
#include "../time.hpp"
#include <glm/glm.hpp>

struct Plane {
  glm::vec3 pt;
  glm::vec3 norm;
  float D;

  Plane();
  Plane(glm::vec3 p, glm::vec3 n);
};

enum FrustumPlanes {
  RIGHT = 0,
  LEFT,
  BOTTOM,
  TOP,
  NEARP,
  FARP,
  NUM_PLANES
};

struct Frustum {
  glm::vec3 origin;
  glm::vec3 direction;
  Plane planes[FrustumPlanes::NUM_PLANES];
  float nearW;
  float nearH;
  float farW;
  float farH;
  float nearDist;
  float farDist;
};

struct Camera {
  glm::dvec3 target;
  glm::dvec3 position;
  glm::dvec3 direction;
  glm::dvec3 up;
  glm::dvec3 right;
  Frustum viewFrustum;
  Line ray;
  glm::vec2 window;
  glm::vec2 projSize;
  glm::vec2 invProjSize;
  double size;
  double distance;
  float resolutionDistance;
  double yaw;
  double pitch;
  double roll;
  float aspectRatio;
  float distanceFactor;
  glm::mat4 view;
  glm::mat4 projection;
  glm::mat4 viewProjection;
  LightInformation light;
  glm::vec3 clearColor;
};

enum WindowMode {
  WindowedFullscreen, Windowed, Fullscreen,
  numWindowModes
};

enum Perspective {
  MainPerspective, SatMapPerspective, CapturePerspective,
  numPerspectives
};

item getMsaaSamples();
void setMsaaSamples(item samples);
bool getCameraClassicRMB();
void setCameraClassicRMB(bool val);
bool getCameraEdgeScrolling();
void setCameraEdgeScrolling(bool val);
int cameraLoop();
float getHorizontalCameraAngle();
void setHorizontalCameraAngle(float angle);
float getVerticalCameraAngle();
void swapCameras();
void resetCamera();
void setCameraTarget(glm::vec3 location);
glm::vec3 getCameraTarget();
void setCameraYaw(float yaw);
void setCameraPitch(float pitch);
void setCameraRoll(float roll);
float getCameraDistance();
void setCameraDistance(float dist);
float getAspectRatio();
Camera getUICamera();
Camera getMainCamera();
Camera getMainCameraBack();
Camera getCaptureCamera();
Camera getCaptureCameraBack();
Camera getMapCamera();
Camera getCurrentCamera_g();
Camera getCurrentCamera_r();

void updateCamera(double duration);
GLFWwindow* getWindow();
void scrollCamera(InputEvent event);
void moveCamera(double duration, InputEvent event);
void draw();
void positionCamera();
int initGraphics();
void resetGraphics();
void setFreezeCamera(bool freezeCamera);
void setWindowMode(WindowMode mode);
WindowMode getWindowMode();
bool inFrustrum(Camera c, glm::vec3 eloc, float eSize);
void setFOV(float newFOV);
float getFOV();
float getFOVBias();
float getFOVMaxDegrees();
float getFOVMaxRadians();

void checkOpenGLError();
void updateWindowMode();
void swapGLBuffers();
void clearColorAndDepth();
void clearSky();
void clearDepth();
void setMainViewport();
void setupShadowCamera(GLuint programID);
void setupLightingCamera(GLuint programID);
void setupWeatherCamera(GLuint programID);
void setupSkyboxCamera(GLuint programID);
void setupMainCamera(GLuint programID);
void setupUICamera(GLuint programID);

void setNextPerspective_g(item perspective);
Perspective getPerspective_g();
Perspective getPerspective_r();


