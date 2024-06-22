#pragma once

#include "part.hpp"

enum TextureSelectionState {
  TSSClosed, TSSAlbedo, TSSIllumination,
  numTextureSelectionStates
};

Part* textureSelectPanel(glm::vec2 loc, glm::vec2 size, item textureSelectionState);
static bool selectTexture(Part* part, InputEvent event);

