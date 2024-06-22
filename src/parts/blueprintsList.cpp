#include "blueprintsList.hpp"

#include "../blueprint.hpp"
#include "../color.hpp"
#include "../icons.hpp"
#include "../string_proxy.hpp"

#include "blueprintPanel.hpp"
#include "button.hpp"
#include "label.hpp"
#include "panel.hpp"
#include "scrollbox.hpp"

const float bpPad = 0.1;
const float bpWidth = 8;
static ScrollState bpScroll;
const glm::vec2 bpPanelSize = glm::vec2(bpWidth+bpPad, 2);

bool selectBlueprint(Part* part, InputEvent event) {
  selectBlueprint(part->itemData);
  return true;
}

Part* blueprintsList(float xSize, float ySize) {
  float usableYSize = ySize - 2;//8;
  Part* scroll = scrollbox(glm::vec2(0,0), glm::vec2(bpWidth, usableYSize));
  scroll->padding = 0;

  float y = 0;
  if (numBlueprints() > 0) {
    for (int i = numBlueprints()-1; i >= 0; i--) {
      Blueprint* bp = getBlueprint(i);
      Part* bpPanel = blueprintPanel(glm::vec2(0.6,y), bpPanelSize,
          bp, getBlueprintTB(i), i);
      bpPanel->onClick = selectBlueprint;
      bpPanel->itemData = i;
      r(scroll, bpPanel);
      y += bpPanelSize.y + .25f;
    }
  } else {
    Part* bpMsg = panel(glm::vec2(0.6,y), bpPanelSize);
    bpMsg->renderMode = RenderPanelGradient;
    bpMsg->texture = line(colorBlueGrad0, colorBlueGrad1);
    bpMsg->padding = bpPad;
    r(bpMsg, labelCenter(glm::vec2(0, 0.05), glm::vec2(bpWidth-bpPad, .9f),
          strdup_s("No Saved Blueprints")));
    r(bpMsg, labelCenter(glm::vec2(0, 1.125), glm::vec2(bpWidth-bpPad, .75f),
          strdup_s("(Use the tool to make one.)")));
    r(scroll, bpMsg);
  }

  Part* scrollFrame = scrollboxFrame(
      glm::vec2(xSize-bpWidth-bpPad-0.7, 1.5-bpPad),
      glm::vec2(bpWidth+bpPad*2+1, usableYSize+bpPad*2),
      &bpScroll, scroll, true);
  scrollFrame->renderMode = RenderTransparent;
  scrollFrame->flags &= ~_partLowered;

  return scrollFrame;
}

