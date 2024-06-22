#include "slider.hpp"

#include "../sound.hpp"

#include "block.hpp"
#include "hr.hpp"

#include <stdio.h>

float sliderPadding = 0.1;
const float palleteSize = 16;

bool sliderCallback(Part* part, InputEvent event) {
  if (!event.isMouse) {
    return false;
  }
  if (event.action == GLFW_PRESS) {
    playSound(_soundClickPrimary);
  } else if (event.action == GLFW_RELEASE) {
    playSound(_soundClickCancel);
  }
  if (!event.isButtonDown[LMB]) {
    return false;
  }

  float value;
  if (part->flags & _partVertical) {
    value = event.mouseLoc.y / part->dim.end.y;
  } else {
    value = event.mouseLoc.x / part->dim.end.x;
  }

  if (value < 0.05) {
    value = 0;
  } else if (value > 0.95) {
    value = 1;
  }
  part->vecData.x = value;

  if (part->onCustom != 0) {
    return part->onCustom(part, event);
  } else {
    return true;
  }
}

Part* slider(glm::vec2 start, glm::vec2 size, float value,
  bool vertical, InputCallback callback
) {
  Part* result = part(start);
  result->dim.end = glm::vec3(size, 0);
  result->onClick = sliderCallback;
  result->onHover = sliderCallback;
  result->flags |= _partHover;
  result->flags |= _partIsPanel;
  result->flags |= _partDisableSounds;
  result->flags &= ~_partFreePtr;
  result->vecData.x = value;
  result->onCustom = callback;
  result->renderMode = RenderDarkBox;

  float sliderY;
  float sliderX;
  float sliderSize;

  if (vertical) {
    sliderSize = size.x - sliderPadding*2;
    sliderX = sliderPadding;
    sliderY = sliderPadding + value*(size.y - size.x);
    result->flags |= _partVertical;

  } else {
    sliderSize = size.y - sliderPadding*2;
    sliderX = sliderPadding + value*(size.x - size.y);
    sliderY = sliderPadding;
  }

  //r(result, darkBlock(glm::vec2(0, 0), glm::vec2(size.x, size.y)));
  r(result, block(glm::vec2(sliderX, sliderY), glm::vec2(sliderSize, sliderSize)));

  return result;
}

Part* slider(glm::vec2 start, glm::vec2 size, float value, InputCallback callback) {
  return slider(start, size, value, false, callback);
}

Part* slider(glm::vec2 start, glm::vec2 size, float value, InputCallback callback,
    Line tex) {
  Part* result = slider(start, size, value, false, callback);
  //textureStart = (textureStart + glm::vec3(0.5,0.5,0))/palleteSize;
  //textureEnd = (textureEnd + glm::vec3(0.5,0.5,0))/palleteSize;
  result->texture = tex;
  result->renderMode = RenderGradient;
  return result;
}

Part* vslider(glm::vec2 start, glm::vec2 size, float value, InputCallback callback) {
  return slider(start, size, value, true, callback);
}
