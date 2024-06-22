#pragma once

const float splashScreenTime = 5;

float getLastDurationError();
bool isRenderLagging();
bool isGameLagging();
void quickDraw();
void renderMap_g();
void renderCapture_g();
bool isMultithreading();
float getCameraTime();
int getFPSCap();
void setFPSCap(int cap);
float getFPS();
float getFrameDuration();
double getTargetFrameDuration_g();
void endGame();
void restartGame();


