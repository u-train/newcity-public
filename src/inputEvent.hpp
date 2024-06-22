#pragma once

#include "line.hpp"
#include <glm/glm.hpp>
// todo: this should be fixed by moving these two requires to a header files.
// It happens way too often that glfw3 gets included first in some chain instead of glew.h.
// Causing some weird compile errors.
#include <GL/glew.h>
#include <glfw3.h>

const int UNICODE_INPUT = -1000;
const int MOUSE_MOVED_INPUT = -1001;
const int SCROLL = -1002;

enum MouseButton { LMB, MMB, RMB };

const int mouseButtonToGLFW[3] = {
  GLFW_MOUSE_BUTTON_LEFT,
  GLFW_MOUSE_BUTTON_MIDDLE,
  GLFW_MOUSE_BUTTON_RIGHT
};

struct InputEvent {
  int button;
  int key;
  int scancode;
  int action;
  int mods;
  int unicode;
  glm::vec2 scroll;
  glm::vec2 mouseLoc;
  glm::vec2 cameraSpaceMouseLoc;
  Line mouseLine;
  bool doubleClick;
  bool isMouse;
  bool isButtonDown[3];
  bool isKeyDown[GLFW_KEY_LAST+1];
};

