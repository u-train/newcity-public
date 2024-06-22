#pragma once

#include "../item.hpp"

#include <vector>

item getGraphParent(item ndx);
void setGraphParent(item child, item parent);
std::vector<item> getGraphChildren(item parent);
void clearGraphChildren(item ndx);
void resetGraphParents();

