#include "test.hpp"
#include "draw/entity.hpp"
#include "game/game.hpp"
#include "land.hpp"
#include "name.hpp"
#include "testGraph.cpp"
#include "route/testRouting.cpp"

#define CATCH_CONFIG_MAIN
#include "game/catch.hpp"

void test() {
  setRenderEnabled(false);
  initNames();
  resetAll();
  generateFlatLand(2);
  testGraph();
  testRouting();
  resetNames();
}

// void main() {}