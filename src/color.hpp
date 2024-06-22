#pragma once

#include "line.hpp"
#include <glm/vec3.hpp>

const int numColors = 32;
const float paletteSize = 64;

enum PickerPalette {
  RedDark, RedMedD, RedMedL, RedLight,
  OrangeDark, OrangeMedD, OrangeMedL, OrangeLight,
  YellowDark, YellowMedD, YellowMedL, YellowLight,
  GreenDark, GreenMedD, GreenMedL, GreenLight,
  CyanDark, CyanMedD, CyanMedL, CyanLight,
  BlueDark, BlueMedD, BlueMedL, BlueLight,
  PurpleDark, PurpleMedD, PurpleMedL, PurpleLight,
  Black, GrayDark, GrayLight, White,
  Transparent, // special
};

glm::vec3 getColorInPalette(int color);
glm::vec3 getTerrainColor(float u, bool desert);
Line getHeatmapGradient(int hm);

const auto colorDarkGray = glm::vec3(11/paletteSize, 5/paletteSize,0);
const auto colorTransparentWhite = glm::vec3(9/paletteSize, 3/paletteSize, 0);
const auto colorWhite = glm::vec3(7/paletteSize, 1/paletteSize, 0);
const auto colorTree = glm::vec3(1/paletteSize,1/paletteSize,0);
const auto colorBrown = glm::vec3(3/paletteSize,1/paletteSize,0);
const auto colorBeige = glm::vec3(5/paletteSize,1/paletteSize,0);
const auto colorPavement = glm::vec3(9/paletteSize,5/paletteSize,0);
const auto colorLightGreen = glm::vec3(5/paletteSize,5/paletteSize,0);
const auto colorBrightGreen = glm::vec3(11/paletteSize,1/paletteSize,0);
const auto colorYellow = glm::vec3(7/paletteSize,5/paletteSize,0);
const auto colorYellowFarm = glm::vec3(13/paletteSize,5/paletteSize,0);
const auto colorDarkSand = glm::vec3(7/paletteSize,5/paletteSize,0);
const auto colorGray = glm::vec3(13/paletteSize,1/paletteSize,0);
const auto colorDensityPavement = glm::vec3(11/paletteSize,3/paletteSize,0);
const auto colorWaterBlue = glm::vec3(1/paletteSize,5/paletteSize,0);
const auto colorRed = glm::vec3(9/paletteSize, 1/paletteSize, 0);
const auto colorBlue = glm::vec3(15/paletteSize, 29/paletteSize, 0);

const auto colorPanelGrad0 = glm::vec3(3/paletteSize,31/paletteSize, 0);
const auto colorPanelGrad1 = glm::vec3(3/paletteSize,29/paletteSize, 0);
const auto colorTransparentBlack = glm::vec3(1/paletteSize,31/paletteSize, 0);
const auto colorTransparent0 = glm::vec3(17/paletteSize,29/paletteSize, 0);
const auto colorTransparent1 = glm::vec3(17/paletteSize,31/paletteSize, 0);
const auto colorDarkGrad0 = glm::vec3(9/paletteSize,31/paletteSize, 0);
const auto colorDarkGrad1 = glm::vec3(9/paletteSize,29/paletteSize, 0);
const auto colorRaisedGrad0 = glm::vec3(7/paletteSize,31/paletteSize, 0);
const auto colorRaisedGrad1 = glm::vec3(7/paletteSize,29/paletteSize, 0);
const auto colorLoweredGrad0 = glm::vec3(5/paletteSize,31/paletteSize, 0);
const auto colorLoweredGrad1 = glm::vec3(5/paletteSize,29/paletteSize, 0);
const auto colorGoldGrad0 = glm::vec3(13/paletteSize,31/paletteSize, 0);
const auto colorGoldGrad1 = glm::vec3(13/paletteSize,29/paletteSize, 0);
const auto colorRedGrad0 = glm::vec3(11/paletteSize,31/paletteSize, 0);
const auto colorRedGrad1 = glm::vec3(11/paletteSize,29/paletteSize, 0);
const auto colorBlueGrad0 = glm::vec3(15/paletteSize,29/paletteSize, 0);
const auto colorBlueGrad1 = glm::vec3(15/paletteSize,31/paletteSize, 0);
const auto colorGreenGrad0 = glm::vec3(5/paletteSize,31/paletteSize, 0);
const auto colorGreenGrad1 = glm::vec3(5/paletteSize,29/paletteSize, 0);
const auto colorNewsGrad0 = glm::vec3(19/paletteSize,29/paletteSize, 0);
const auto colorNewsGrad1 = glm::vec3(19/paletteSize,31/paletteSize, 0);
const auto colorNewsDarkGrad0 = glm::vec3(21/paletteSize,29/paletteSize, 0);
const auto colorNewsDarkGrad1 = glm::vec3(21/paletteSize,31/paletteSize, 0);

