#include "span.hpp"

#include "label.hpp"

#include "../string.hpp"
#include "../string_proxy.hpp"

#include "spdlog/spdlog.h"
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
using namespace std;

Part* spanLine(glm::vec2 start, glm::vec2 size, string line, bool center) {
  char* str = strdup_s(line.c_str());
  Part* l = label(start, size, str);
  l->lineHeight = size.y;
  if (center) {
    l->flags |= _partAlignCenter;
  } else {
    l->dim.end.x = stringWidth(str)*size.y;
  }
  l->padding = 0;
  return l;
}

Part* span(glm::vec2 start, float indent, glm::vec2 size, char* text, glm::vec2* end,
    bool center) {
  Part* result = part(glm::vec2(0, 0));
  float padding = size.y*0.125;
  result->padding = 0;
  result->renderMode = RenderSpan;
  float ySizeLine = (size.y - padding*2);
  float lineSpacing = size.y;
  float xSizeLine = (size.x - padding*2);
  glm::vec2 cursor = start;
  std::string str(text);

  if (cursor.x >= xSizeLine) {
    cursor.x = indent;
    cursor.y += lineSpacing;
  }

  boost::char_separator<char> sepNewLine("\n", "", boost::keep_empty_tokens);
  tokenizer lineTokens(str, sepNewLine);
  std::string currentLine;

  BOOST_FOREACH(std::string const& lineToken, lineTokens) {
    boost::char_separator<char> sepSpace(" ", "", boost::keep_empty_tokens);
    tokenizer wordTokens(lineToken, sepSpace);
    bool firstOne = true;

    BOOST_FOREACH(std::string const& wordToken, wordTokens) {
      item subNdx = 0;
      item wordChars = wordToken.length();
      //while (true) {
        std::string subWordToken = wordToken;
        /*
        std::string subWordToken;
        item endChar = wordChars+1;
        do {
          endChar --;
          subWordToken = wordToken.substr(subNdx, endChar - subNdx);
        } while (endChar > subNdx+1 &&
            std::stringWidth(subWordToken.c_str())*ySizeLine + cursor.x > xSizeLine);
            */

        if (!firstOne) currentLine += " ";
        std::string next = currentLine + subWordToken;
        //if (next.length() <= 0) next = " ";
        //SPDLOG_INFO("token: |{}| => |{}|", subWordToken, next);
        float strLength = stringWidth(next.c_str())*ySizeLine;
        if (strLength + cursor.x < xSizeLine) {
          currentLine = next;
        } else {
          rtrim(currentLine);
          glm::vec2 size = glm::vec2(xSizeLine - cursor.x, ySizeLine);
          r(result, spanLine(cursor, size, currentLine, center));
          currentLine = subWordToken;
          cursor.x = indent;
          cursor.y += lineSpacing;
        }
        firstOne = false;

        //subNdx = endChar;
        //if (endChar == wordChars) break;
      //}
    }

    if (currentLine.length() > 0) {
      glm::vec2 size = glm::vec2(xSizeLine - cursor.x, ySizeLine);
      r(result, spanLine(cursor, size, currentLine, center));
      currentLine = "";
      cursor.x = indent;
      cursor.y += lineSpacing;
    }
  }

  if (result->numContents == 0) {
    *end = start;
  } else if (center) {
    *end = cursor;
  } else {
    Part* last = result->contents[result->numContents-1];
    glm::vec2 lastCursor = glm::vec2(last->dim.start);
    lastCursor.x += stringWidth(last->text)*ySizeLine;
    *end = lastCursor;
  }

  free(text);

  return result;
}

Part* span(glm::vec2 start, float indent, glm::vec2 size, char* text, glm::vec2* end) {
  return span(start, indent, size, text, end, false);
}

Part* span(glm::vec2 start, float indent, glm::vec2 size, char* text, float* y) {
  glm::vec2 end;
  Part* result = span(start, indent, size, text, &end, false);
  *y = end.y;
  return result;
}

Part* spanCenter(glm::vec2 start, float indent, glm::vec2 size, char* text, glm::vec2* end) {
  return span(start, indent, size, text, end, true);
}

Part* spanCenter(glm::vec2 start, float indent, glm::vec2 size, char* text, float* y) {
  glm::vec2 end;
  Part* result = span(start, indent, size, text, &end, true);
  *y = end.y;
  return result;
}

