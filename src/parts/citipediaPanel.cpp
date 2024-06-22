#include "citipediaPanel.hpp"

#include "article.hpp"
#include "button.hpp"
#include "hr.hpp"
#include "icon.hpp"
#include "label.hpp"
#include "leftPanel.hpp"
#include "panel.hpp"
#include "scrollbox.hpp"
#include "span.hpp"

#include "../color.hpp"
#include "../icons.hpp"
#include "../string_proxy.hpp"
#include "../time.hpp"
#include "../util.hpp"

#include "spdlog/spdlog.h"

const char* citipediaArticleCode404 = "404";

const std::string citipedia404Title = "Missing Article";
const std::string citipedia404Content =
"Oops! That article is missing.\n"
"![](docs/images/missing.png)";

static ScrollState citipediaScroll;
static char* currentArticle = 0;
static std::vector<char*> history;
static std::vector<ScrollState> scrollHistory;
static item historyIndex = -1;

const float cpHeight = 18.5;
const float cpWidth = 18.5;
const float cpPad = 0.25;
const float cpScale = 0.7;

const char* citipediaGetArticleCode404() {
  return citipediaArticleCode404;
}

std::string citipediaGet404Title() {
  return citipedia404Title;
}

std::string citipediaGet404Content() {
  return citipedia404Content;
}

bool closeCitipediaPanel(Part* part, InputEvent event) {
  setLeftPanel(NoPanel);
  return true;
}

bool shiftHistory(Part* part, InputEvent event) {
  item amount = part->itemData;
  historyIndex = iclamp(historyIndex+amount, 0, history.size()-1);
  currentArticle = history[historyIndex];
  citipediaScroll = scrollHistory[historyIndex];
  return true;
}

void pushHistory() {
  if (currentArticle != 0) {
    if (historyIndex < history.size()-1) {
      history.erase(history.begin()+historyIndex+1, history.end());
      scrollHistory.erase(scrollHistory.begin()+historyIndex+1,
          scrollHistory.end());
    }
    history.push_back(currentArticle);

    if (scrollHistory.size() > 0) {
      scrollHistory.back() = citipediaScroll;
    }
    scrollHistory.push_back(citipediaScroll);

    historyIndex ++;
    historyIndex = iclamp(historyIndex, 0, history.size()-1);
  }
}

void selectCitipediaArticle(char* link) {
  currentArticle = link;
  setLeftPanel(CitipediaPanel);
  pushHistory();
  resetScrollState(&citipediaScroll);
}

Part* citipediaPanel() {
  glm::vec2 cpLoc = glm::vec2(0.1, 1.5);
  glm::vec2 cpSize = glm::vec2(cpWidth+cpPad*2, cpHeight+cpPad*2);
  Part* result = panel(cpLoc, cpSize);
  result->padding = cpPad;
  float titleSpacing = 1.2;

  if (currentArticle == 0) {
    currentArticle = strdup_s("index");
    pushHistory();
  }

  r(result, icon(glm::vec2(0,0), iconCitipedia));

  Part* backButt = r(result, button(glm::vec2(cpWidth-4,0), iconLeft, glm::vec2(1,1),
        shiftHistory, -1));
  if (historyIndex <= 0) backButt->foregroundColor = PickerPalette::GrayDark;

  Part* homeButt = r(result, button(glm::vec2(cpWidth-3,0), iconHouse, glm::vec2(1,1),
        followLink, 0));
  homeButt->ptrData = strdup_s("index");
  homeButt->flags |= _partFreePtr;

  Part* foreButt = r(result, button(glm::vec2(cpWidth-2,0), iconRight, glm::vec2(1,1),
        shiftHistory, 1));
  if (historyIndex >= history.size()-1) {
    foreButt->foregroundColor = PickerPalette::GrayDark;
  }

  r(result, button(glm::vec2(cpWidth-1,0), iconX, glm::vec2(1,1),
        closeCitipediaPanel, 0));

  ArticleRenderConfig config;
  config.flags = 0;
  config.textColor = PickerPalette::White;
  config.captionTextColor = PickerPalette::White;
  config.linkTextColor = PickerPalette::CyanLight;
  config.captionBackground = line(colorDarkGrad0, colorDarkGrad1);
  config.linkHandler = followLink;
  config.chartDate = getCurrentDateTime();

  Part* scroll = scrollbox(glm::vec2(0,0), glm::vec2(cpWidth-1, cpHeight-titleSpacing));
  char* title = 0;
  r(scroll, article(glm::vec2(0,0), glm::vec2(cpWidth-1, cpScale),
        &currentArticle, &title, config));

  float widthForTitle = cpWidth-4.25;
  glm::vec2 titleEnd(0,0);
  Part* titlePart = r(result, spanCenter(glm::vec2(1,0), 1, glm::vec2(widthForTitle+0.5, titleSpacing), title == 0 ? strdup_s("Citipedia") : title, &titleEnd));

  // Align vertically
  float scrollBoxY = std::max(titleSpacing, titleEnd.y);

  Part* scrollFrame = scrollboxFrame(glm::vec2(0,scrollBoxY),
      glm::vec2(cpWidth, cpHeight-scrollBoxY),
      &citipediaScroll, scroll);
  r(result, scrollFrame);

  return result;
}

