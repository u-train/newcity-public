#pragma once

#include "../item.hpp"
#include <cstdint>
#include <vector>
#include <string>

void convertDesignToPackage(item designNdx);
void addTextureToPackage(item designNdx, item texture, bool isIllum);
void deleteTextureInPackage(item designNdx, item texture);
void assignBuildingTexture(item buildingNdx);
item numMatchingTexturesForBuilding(item ndx);
void deleteDesignPackageTexture(item buildingNdx, bool illum);
void loadDesignPackageTextures();
void loadDesignPackageTextures(std::string packageName, item designNdx, uint32_t loadFlags);
void cleanupLegacyDesignFiles();
void deleteDesign(item ndx);
void resetDesignPackages();
std::vector<item> getAllPackageTextures(bool illum);
bool designHasPackageTextures(item designNdx);
void linkDesignPackage(item source, item dest);
void designPackageResaved();

