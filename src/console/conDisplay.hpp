//-----------------------------------------------------------------------------
// conDisplay.hpp - Contains the data to be displayed by the
// console, and the functions to modify it
//-----------------------------------------------------------------------------

#pragma once

#include <glm/glm.hpp>
#include <stdint.h>
#include <string>

const float conPad = 0.1f;
const float conScaleTitle = 0.8f;
const float conScaleText = 0.6f;
const int conMinSizeX = 5;
const int conMinSizeY = 5;

std::string consoleGetText();
uint32_t consoleGetNumLines();
bool consolePrint(std::string data);
bool consolePrintLine(std::string data);
bool consolePrintDebug(std::string data, bool log = true);
bool consolePrintError(const char *err, std::string info);
bool consoleEchoInput(std::string input);
void consoleClear();
void consoleSetSize(float x, float y);
glm::vec2 consoleSize();
void consoleSetSize(glm::vec2 newSize);
glm::vec2 consolePos();
void consoleSetPos(glm::vec2 newPos);
void consoleToggle();
void consoleSetOpen(bool open);
bool consoleIsOpen();
