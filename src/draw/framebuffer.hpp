#pragma once

#include <glm/vec2.hpp>
#include <string>

enum Framebuffer {
  MapFramebuffer, CaptureFramebuffer,
  ShadowFramebuffer, LightingFramebuffer, MainFramebuffer,
  numFramebuffers
};

void initFramebuffers();
void setupFramebuffer(Framebuffer f);
unsigned int getMapTexture();
void generateMapMipMap();
void captureFramebuffer(Framebuffer f, std::string filename);
void captureFramebuffer();
glm::vec2 getCaptureDimensions();
void swapFramebuffers();

