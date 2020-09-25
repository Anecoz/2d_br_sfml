#include <iostream>

#include "FontCache.h"
#include "Game.h"

int main()
{
  if (!fontcache::init()) {
    std::cout << "Could not init fontcache!" << std::endl;
    return 1;
  }

  Game game;
  if (!game.init()) {
    std::cout << "Game could not init!" << std::endl;
    return 2;
  }

  game.run();

  return 0;
}