#pragma once

#include <SFML/Graphics.hpp>

#include "net/NetClient.h"
#include "Player.h"

class Game
{
public:
  Game();
  ~Game() = default;

  bool init();
  void run();

private:
  sf::RenderWindow _window;
  net::NetClient _netClient;
  Player _player;
};