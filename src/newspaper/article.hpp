#pragma once

#include "newspaper.hpp"

#include <string>

void resetNewspaperArticles();
void initNewspaperLua();
std::vector<NewspaperIssueArticle> generateNewspaperArticles(item num);
void forceNewspaperArticle(std::string code);

std::string getAdArticle(item ndx);
item numAdArticles();

