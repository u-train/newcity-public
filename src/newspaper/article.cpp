#include "article.hpp"

#include "../item.hpp"
#include "../platform/lua.hpp"
#include "../pool.hpp"
#include "../error.hpp"
#include "../string_proxy.hpp"

#include "../md4c/md4c.h"

#include "../platform/lookup.hpp"
#include "../parts/image.hpp"
#include "../util.hpp"
#include "../game/constants.hpp"

#include "spdlog/spdlog.h"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

struct NewspaperArticle {
  uint32_t flags;
  char* code;
  char* file;
  item group;
  int scoreFunc;
};

struct NewspaperArticleGroup {
  uint32_t flags;
  char* code;
};

const int _articleExists = 1 << 0;

const int _articleGroupExists = 1 << 0;

Pool<NewspaperArticle> articles;
Pool<NewspaperArticle> adArticles;
Pool<NewspaperArticleGroup> articleGroups;
std::unordered_map<std::string, item> articleGroupsByCode;
std::unordered_map<std::string, item> articlesByCode;
std::vector<std::string> forcedArticles;


//const uint32_t _cursorWebLink        = 1 << 0;

struct ArticleCursor {
  uint32_t flags = 0;
  const char* filename;
  std::string result;
  std::string imageSrc;
};

const uint32_t _cursorWebLink        = 1 << 0;
const uint32_t _cursorMath           = 1 << 1;
const uint32_t _cursorUnderline      = 1 << 2;
const uint32_t _cursorStrikethrough  = 1 << 3;
const uint32_t _cursorCaption        = 1 << 4;
const uint32_t _cursorCaptionApplied = 1 << 5;

FileBuffer readNewspaperArticle(const char* articleFile) {
  std::string fn = lookupFile(articleFile, 0);
  FileBuffer result = readFromFile(fn.c_str(), true);

  if (result.length > 0) {
    return result;
  } else {
    return readNewspaperArticle("newspaper/404.md");
  }
}

void transformImage(ArticleCursor* cursor, MD_SPAN_IMG_DETAIL* det) {
  char* src = strdup_s(det->src.text, det->src.size);
  if (src == 0) return;
  if (strlength(src) <= 0) {
    free(src);
    return;
  }

  cursor->result += "![";
  cursor->imageSrc = src;
}

void transformImageCaption(ArticleCursor* cursor) {
  if (cursor->imageSrc.length() != 0) {
    char* fn = strdup_s(cursor->imageSrc.c_str());

    selectFrameInFilename(fn, randItem(10000));
    cursor->result += "](";
    cursor->result += fn;
    cursor->result += ")";
    cursor->imageSrc = "";
    free(fn);
  }
}

int transformEnterBlock(MD_BLOCKTYPE type, void* detail, void* userdata) {
  ArticleCursor* cursor = (ArticleCursor*) userdata;
  if (cursor->result.length() > 0) cursor->result += "\n\n";

  switch(type) {
      case MD_BLOCK_DOC:      break;
      case MD_BLOCK_QUOTE:    {
        cursor->result += "> ";
      } break;

      case MD_BLOCK_UL:       break;
      case MD_BLOCK_OL:       break;

      case MD_BLOCK_LI:       {
        cursor->result += "* ";
      } break;

      case MD_BLOCK_HR:       {
        cursor->result += "---";
      } break;

      case MD_BLOCK_H:        {
        float level = ((MD_BLOCK_H_DETAIL*)detail)->level;
        for (int i = 0; i < level; i++) {
          cursor->result += "#";
        }
        cursor->result += " ";
      } break;

      case MD_BLOCK_CODE:     break;
      case MD_BLOCK_HTML:     break;
      case MD_BLOCK_P:        break;
      case MD_BLOCK_TABLE:    break;
      case MD_BLOCK_THEAD:    break;
      case MD_BLOCK_TBODY:    break;
      case MD_BLOCK_TR:       break;
      case MD_BLOCK_TH:       break;
      case MD_BLOCK_TD:       break;
  }

  return 0;
}

int transformLeaveBlock(MD_BLOCKTYPE type, void* detail, void* userdata) {
  ArticleCursor* cursor = (ArticleCursor*) userdata;

  switch(type) {
      case MD_BLOCK_DOC:      break;
      case MD_BLOCK_QUOTE:    break;
      case MD_BLOCK_UL:       break;
      case MD_BLOCK_OL:       break;
      case MD_BLOCK_LI:       break;
      case MD_BLOCK_HR:       break;
      case MD_BLOCK_H:        break;
      case MD_BLOCK_CODE:     break;
      case MD_BLOCK_HTML:     break;
      case MD_BLOCK_P:        break;
      case MD_BLOCK_TABLE:    break;
      case MD_BLOCK_THEAD:    break;
      case MD_BLOCK_TBODY:    break;
      case MD_BLOCK_TR:       break;
      case MD_BLOCK_TH:       break;
      case MD_BLOCK_TD:       break;
  }

  return 0;
}

int transformEnterSpan(MD_SPANTYPE type, void* detail, void* userdata) {
  ArticleCursor* cursor = (ArticleCursor*) userdata;

  switch(type) {
    case MD_SPAN_EM:                { cursor->result += "*"; } break;
    case MD_SPAN_STRONG:            { cursor->result += "**"; } break;
    case MD_SPAN_U:                 { cursor->result += "_"; } break;
    case MD_SPAN_A:                 break;

    case MD_SPAN_IMG:               {
      transformImage(cursor, (MD_SPAN_IMG_DETAIL*)detail);
    } break;

    case MD_SPAN_CODE:              break;
    case MD_SPAN_DEL:               { cursor->result += "~"; } break;
    case MD_SPAN_LATEXMATH:         { cursor->flags |= _cursorMath; } break;
    case MD_SPAN_LATEXMATH_DISPLAY: { cursor->flags |= _cursorMath; } break;
    case MD_SPAN_WIKILINK:          break;
  }

  return 0;
}

int transformLeaveSpan(MD_SPANTYPE type, void* detail, void* userdata) {
  ArticleCursor* cursor = (ArticleCursor*) userdata;

  switch(type) {
    case MD_SPAN_EM:                { cursor->result += "*"; } break;
    case MD_SPAN_STRONG:            { cursor->result += "**"; } break;
    case MD_SPAN_U:                 { cursor->result += "_"; } break;
    case MD_SPAN_A:                 break;

    case MD_SPAN_IMG:               {
      transformImageCaption(cursor);
    } break;

    case MD_SPAN_CODE:              break;
    case MD_SPAN_DEL:               { cursor->result += "~"; } break;
    case MD_SPAN_LATEXMATH:         { cursor->flags &= ~_cursorMath; } break;
    case MD_SPAN_LATEXMATH_DISPLAY: { cursor->flags &= ~_cursorMath; } break;
    case MD_SPAN_WIKILINK:          break;
  }

  return 0;
}

void transformTextInner(MD_TEXTTYPE type, char* text, ArticleCursor* cursor) {
  if (type == MD_TEXT_LATEXMATH) {
    char* code = text;
    text = interpretLua(code, cursor->filename);
    free(code);
  }

  if (text == 0) return;
  if (strlength(text) <= 0) {
    free(text);
    return;
  }

  cursor->result += text;

  //if (type == MD_TEXT_LATEXMATH) { // always loses a space
    //cursor->result += " ";
  //}

  if (cursor->flags & _cursorCaption) {
    cursor->flags |= _cursorCaptionApplied;
  }
}

int transformText(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size,
    void* userdata) {
  ArticleCursor* cursor = (ArticleCursor*) userdata;

  switch(type) {
    case MD_TEXT_NULLCHAR:  break;
    case MD_TEXT_BR:        // fallthrough
    case MD_TEXT_SOFTBR:    {
      cursor->result += "\n\n";
    } break;
    case MD_TEXT_HTML:      break;
    case MD_TEXT_ENTITY:    break;
    default:                {
      transformTextInner(type, strdup_s(text, size), cursor);
    } break;
  }

  return 0;
}

void transformDebug(const char* msg, void* userdata) {
  SPDLOG_WARN("Markdown Parsing Error: {}", msg);
}

std::string transformArticleToIssueArticleInner(std::string data, char* file) {
  ArticleCursor cursor;
  cursor.filename = file;

  MD_PARSER parser;
  parser.abi_version = 0;
  parser.enter_block = transformEnterBlock;
  parser.leave_block = transformLeaveBlock;
  parser.enter_span = transformEnterSpan;
  parser.leave_span = transformLeaveSpan;
  parser.text = transformText;
  parser.debug_log = transformDebug;
  parser.syntax = 0;

  parser.flags =
    MD_FLAG_STRIKETHROUGH |
    MD_FLAG_UNDERLINE |
    //MD_FLAG_WIKILINKS |
    MD_FLAG_NOHTML |
    MD_FLAG_COLLAPSEWHITESPACE |
    MD_FLAG_LATEXMATHSPANS |
    //MD_FLAG_PERMISSIVEURLAUTOLINKS |
    //MD_FLAG_PERMISSIVEWWWAUTOLINKS |
    0;

  md_parse(data.c_str(), data.length(), &parser, (void*) &cursor);
  return cursor.result;
}

NewspaperIssueArticle transformArticleToIssueArticle
    (NewspaperArticle* article) {

  FileBuffer file = readNewspaperArticle(article->file);

  std::string data = file.data;
  std::string resStr = transformArticleToIssueArticleInner(data, article->file);
  freeBuffer(&file);

  const char* text = resStr.c_str();
  int curs = 0;
  for (;text[curs] != '\0' && text[curs] != '\n'; curs++);

  char* title = strdup_s(text, curs);

  char* resText = strdup_s(text + curs + 1);

  NewspaperIssueArticle result;
  result.code = strdup_s(article->code);
  result.text = resText;
  result.title = title;
  return result;
}

void resetNewspaperArticles() {
  // clean up strings
  for (int i=1; i <= articles.size(); i++) {
    NewspaperArticle* article = articles.get(i);
    if (article->code) {
      free(article->code);
    }
    if (article->file) {
      free(article->file);
    }
  }
  articles.clear();

  for (int i=1; i <= adArticles.size(); i++) {
    NewspaperArticle* article = adArticles.get(i);
    if (article->code) {
      free(article->code);
    }
    if (article->file) {
      free(article->file);
    }
  }
  adArticles.clear();
}

int addNewspaperArticleGroup(lua_State* L) {
  int numArgs = lua_gettop(L);
  if (numArgs <= 0) return 0;

  item ndx = articleGroups.create();
  NewspaperArticleGroup* group = articleGroups.get(ndx);
  group->code = luaFieldString(L, "code");
  group->flags = _articleGroupExists;

  articleGroupsByCode[group->code] = ndx;
  return 0;
}

int addNewspaperArticle(lua_State* L) {
  int numArgs = lua_gettop(L);
  if (numArgs <= 0) return 0;

  char* code = luaFieldString(L, "code");
  item ndx = articlesByCode[code];
  if (ndx == 0) {
    ndx = articles.create();
    articlesByCode[code] = ndx;
  }

  NewspaperArticle* article = articles.get(ndx);
  article->flags = _articleExists;
  article->code = code;
  article->file = luaFieldString(L, "file");
  article->scoreFunc = luaFieldFunction(L, "score");

  if (article->code == 0) {
    handleError("Must provide code for article");
  }

  if (article->file == 0) {
    handleError("Must provide file for article %s", article->code);
  }

  if (article->scoreFunc == -1) {
    handleError("Must provide score function for article %s", article->code);
  }

  char* groupCode = luaFieldString(L, "group");
  if (groupCode == 0) {
    handleError("Must provide group for article %s", article->code);
  }
  article->group = articleGroupsByCode[groupCode];
  if (article->group == 0) {
    handleError("Group %s not found for article %s", groupCode, article->code);
  }
  if (groupCode != 0) free(groupCode);

  return 0;
}

void initNewspaperAds() {
  // init ads
  uint32_t lookupFlags = c(CDisableDefaultNewspaperArticles) ? _lookupOnlyMods : 0;
  std::vector<std::string> dir = lookupDirectory("newspaper/ads/", ".md", lookupFlags);

  for (int i = 0; i < dir.size(); i ++) {
    item adNdx = adArticles.create();
    NewspaperArticle* article = adArticles.get(adNdx);

    std::string file = dir[i];
    std::string code = "Ad_" + file;
    std::string filepath = lookupFile("newspaper/ads/" + file + ".md", lookupFlags).c_str();

    article->flags = _articleExists;
    article->code = strdup_s(code.c_str());
    article->file = strdup_s(filepath.c_str());
    article->scoreFunc = 0;
  }
}

void initNewspaperLua() {
  resetNewspaperArticles();
  articleGroups.clear();
  articleGroupsByCode.clear();
  articlesByCode.clear();
  forcedArticles.clear();
  articles.clear();
  adArticles.clear();
  addLuaCallback("addNewspaperArticle", addNewspaperArticle);
  addLuaCallback("addNewspaperArticleGroup", addNewspaperArticleGroup);
  initNewspaperAds();
}

void forceNewspaperArticle(std::string code) {
  forcedArticles.push_back(code);
  generateNewspaperIssue();
}

std::vector<NewspaperIssueArticle> generateNewspaperArticles(item num) {
  std::vector<float> scores;
  std::vector<NewspaperArticle*> resultArticles;
  std::vector<NewspaperIssueArticle> results;
  scores.reserve(articles.size()+1);
  scores.push_back(-1);

  // Get scores for each article
  for (int i = 1; i <= articles.size(); i++) {
    NewspaperArticle* article = articles.get(i);
    float baseScore = callLuaScoringFunction(article->scoreFunc, article->code);
    float score = baseScore * (1 + randFloat() * c(CNewspaperRandomness));
    if (c(CLogNewspaperData)) {
      SPDLOG_INFO("article {} scored {}, randomized to {}",
          article->code, baseScore, score);
    }

    for (int k = 0; k < forcedArticles.size(); k++) {
      if (streql(forcedArticles[k].c_str(), article->code)) {
        resultArticles.push_back(article);
        score = -1;
      }
    }

    scores.push_back(score);
  }

  forcedArticles.clear();

  // Don't publish the same article over and over
  for (int i = 0; i < 4; i++) {
    int issueNdx = getNumNewspaperIssues()-i-1;
    if (issueNdx < 0) break;

    NewspaperIssue* issue = getNewspaperIssue(issueNdx);
    for (int a = 0; a < issue->articles.size(); a ++) {
      const char* code = issue->articles[a].code;
      if (code == 0) continue;
      item articleNdx = articlesByCode[code];
      if (articleNdx <= 0 || articleNdx >= scores.size()) continue;
      scores[articleNdx] = glm::clamp(scores[articleNdx], -1.f, 0.5f);
    }
  }

  // Get the highest scoring articles, but no two from the same group
  for (int k = resultArticles.size(); k < num; k++) {
    float highScore = -1;
    NewspaperArticle* best = 0;
    item bestNdx = 0;
    for (int i = 1; i <= articles.size(); i++) {
      if (scores[i] > highScore) {

        NewspaperArticle* article = articles.get(i);
        for (int r = 0; r < resultArticles.size(); r ++) {
          if (resultArticles[r]->group == article->group) {
            scores[i] = -1;
            break;
          }
        }

        if (scores[i] > highScore) {
          best = article;
          bestNdx = i;
          highScore = scores[i];
        }
      }
    }

    if (best != 0) {
      scores[bestNdx] = -1;
      if (c(CLogNewspaperData)) {
        SPDLOG_INFO("pushing article {}:{}", bestNdx, best->code);
      }
      resultArticles.push_back(best);
    }
  }

  for (int k = 0; k < resultArticles.size(); k ++) {
    NewspaperArticle* article = resultArticles[k];
    NewspaperIssueArticle issueArticle =
      transformArticleToIssueArticle(article);
    results.push_back(issueArticle);
  }

  return results;
}

std::string getAdArticle(item ndx) {
  if (ndx <= 0 || ndx > adArticles.size()) return "";

  NewspaperArticle* article = adArticles.get(ndx);
  FileBuffer file = readNewspaperArticle(article->file);
  std::string data = file.data;
  // SPDLOG_INFO("getAdArticle data str: {}", data);
  trim(data);

  return transformArticleToIssueArticleInner(data, article->file);
}

item numAdArticles() {
  return adArticles.size();
}

