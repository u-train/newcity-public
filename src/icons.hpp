#pragma once

#include "line.hpp"
#include <glm/glm.hpp>

Line iconToSpritesheet(glm::vec3 icon);
Line iconToSpritesheet(glm::vec3 icon, float wind);
glm::vec3 getIcon(const char* ico);
void initIconsMap();

const float spriteSheetSize = 32.f;
const float spriteResolution = 64.f;

#define ICO(N,V) const glm::vec3 icon##N = V;
#include "iconsEnum.hpp"
#undef ICO

enum Icons {
  #define ICO(N,V) Icon##N,
  #include "iconsEnum.hpp"
  #undef ICO
  numIcons
};

const glm::vec3 expwySignStart = glm::vec3(12, 13, 1);
const glm::vec3 expwySignEnd = glm::vec3(15, 15, 1);

const glm::vec3 iconTransitTypes[] = {
  iconNo,
  iconBus,
  iconTrain,
  iconMonorail,
  iconMonoHang,
  iconRubberTyred,
  iconMagLev,
};

const glm::vec3 iconTransitTypeSigns[] = {
  iconNo,
  iconBusSign,
  iconTrainSign,
  iconMonorailSign,
  iconMonoHangSign,
  iconRubberTyredSign,
  iconMagLevSign,
};

const glm::vec3 iconTransitPowers[] = {
  iconNo,
  iconFuel,
  iconGas,
  iconElectricCatenary,
  iconPulley,
  iconWheels,
  iconInduction,
};

const glm::vec3 iconTransitAutomations[] = {
  iconNo,
  iconPersonMan,
  iconPersonTech,
  iconTech,
  iconTransitCar,
};

const int numTransitLogos = 86;
const int iconNewTrackNdx = 85;
const glm::vec3 iconTransitLogos[] = {
  iconBus,
  iconTrain,
  iconMonorail,
  iconMonoHang,
  iconRubberTyred,
  iconMagLev,

  iconBusSign,
  iconTrainSign,
  iconMonorailSign,
  iconMonoHangSign,
  iconRubberTyredSign,
  iconMagLevSign,

  iconFuel,
  iconGas,
  iconElectricCatenary,
  iconPulley,
  iconWheels,
  iconInduction,

  iconBusSide,
  iconTarget,
  iconTransitCar,
  iconLevitation,
  iconThreeArrows,
  iconRadio,
  iconFlag,

  iconRoute,
  iconFastTrain,
  iconLoop,
  iconFlow,
  iconLeftRight,
  iconUpDown,
  iconBusSideFast,
  iconFighter,

  iconBusiness,
  iconOffice,
  iconPersonMan,
  iconFamily,
  iconWeatherLightning,
  iconWeatherSun,
  iconWeatherMoon,
  iconCash,
  iconCheck,
  iconX,
  iconTree,
  iconTech,
  iconWait,
  iconPencil,
  iconPin,

  //Zone Brush icons
  iconVerticalBar,
  iconDotSmall,
  iconDotMedium,
  iconDotLarge,
  iconPointer,

  // Turn symbols
  iconTurnRight,
  iconTurnStraight,
  iconTurnRightStraight,
  iconTurnLeft,
  iconTurnLeftRight,
  iconTurnLeftStraight,
  iconTurnLeftRightStraight,
  iconTurnU,
  iconTurnURight,
  iconTurnUStraight,
  iconTurnURightStraight,
  iconTurnULeft,
  iconTurnULeftRight,
  iconTurnULeftStraight,
  iconTurnULeftRightStraight,

  iconQuery,
  iconWrench,
  iconGun,
  iconOneWaySignRight,
  iconMergeLeft,
  iconMergeRight,
  iconI,
  iconEye,
  iconChart,
  iconUp,
  iconDown,
  iconRight,
  iconLeft,

  iconSpeedSlow,
  iconSpeedPlay,
  iconSpeedFF2,
  iconSpeedFF3,

  iconNewTrack,
};

const glm::vec3 iconZoneBrushType[] = {
  iconVerticalBar,
  iconDotSmall,
  iconDotMedium,
  iconDotLarge,
  iconPointer,
};

const glm::vec3 iconIntersectionStrategy[] = {
  iconStopSign,
  iconStopLight,
};

const glm::vec3 iconGameSpeed[] = {
  iconSpeedPause,
  iconSpeedSlow,
  iconSpeedPlay,
  iconSpeedFF2,
  iconSpeedFF3,
};

const glm::vec3 iconZone[] = {
  iconZoneNone,
  iconZoneResidential,
  iconZoneRetail,
  iconZoneFarm,
  iconZoneGovernment,
  iconZoneOffice,
  iconZoneFactory,
  iconZoneMixedUse,
  iconZonePark,
};

const glm::vec3 iconZoneMono[] = {
  iconZoneMonoNone,
  iconZoneMonoResidential,
  iconZoneMonoRetail,
  iconZoneMonoFarm,
  iconZoneMonoGovernment,
  iconZoneMonoOffice,
  iconZoneMonoFactory,
  iconZoneMonoMixedUse,
  iconZoneMonoPark,
};

const glm::vec3 iconZoneColor[] = {
  iconZoneColorNone,
  iconZoneColorResidential,
  iconZoneColorRetail,
  iconZoneColorFarm,
  iconZoneColorGovernment,
  iconZoneColorOffice,
  iconZoneColorFactory,
  iconZoneColorMixedUse,
  iconZoneColorPark,
};

const glm::vec3 iconBusinessType[] = {
  iconZoneMonoRetail,
  iconZoneMonoOffice,
  iconZoneMonoFarm,
  iconZoneMonoFactory,
  iconZoneMonoGovernment,
};

const glm::vec3 iconBuildingCategory[] = {
  iconEducationCategory,
  iconParksCategory,
  iconServicesCategory,
  iconUniversityCategory,
  iconOffice,
};

const glm::vec3 iconHeatmap[] = {
  iconPollution,
  iconLandValue,
  iconDensity,
  iconCrime,
  iconEducation,
  iconTrade,
  iconFamily,
  iconHealth
};

const glm::vec3 iconHeatmapColor[] = {
  iconHMBlockPollution,
  iconHMBlockLandValue,
  iconHMBlockDensity,
  iconHMBlockCrime,
  iconHMBlockEducation,
  iconHMBlockProsperity,
  iconHMBlockCommunity,
  iconHMBlockHealth,
};

const glm::vec3 iconSpeedLimitMph[] = {
  iconSpeedLimitSign15MPH,
  iconSpeedLimitSign30MPH,
  iconSpeedLimitSign45MPH,
  iconSpeedLimitSign60MPH,
  iconSpeedLimitSign75MPH,
};

const glm::vec3 iconSpeedLimitKmph[] = {
  iconSpeedLimitSign25KMPH,
  iconSpeedLimitSign50KMPH,
  iconSpeedLimitSign75KMPH,
  iconSpeedLimitSign100KMPH,
  iconSpeedLimitSign125KMPH,
};

const glm::vec3 iconBridgePillar[] = {
  iconTrussBridgePillar,
  iconSuspensionBridgePillar,
};

// iconTurn[isRight][isStraight][isLeft][isU]
// iconTurn[lane->flags & _laneTurnMask >> _laneTurnShift];
const glm::vec3 iconTurn[2*2*2*2] = {
  glm::vec3(0,12,1), glm::vec3(1,12,1), glm::vec3(2,12,1), glm::vec3(3,12,1),
  glm::vec3(0,13,1), glm::vec3(1,13,1), glm::vec3(2,13,1), glm::vec3(3,13,1),
  glm::vec3(0,14,1), glm::vec3(1,14,1), glm::vec3(2,14,1), glm::vec3(3,14,1),
  glm::vec3(0,15,1), glm::vec3(1,15,1), glm::vec3(2,15,1), glm::vec3(3,15,1),
};

