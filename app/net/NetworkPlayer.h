#pragma once

#include "PlayerState.h"

#include <SFML/Graphics.hpp>

namespace net {

class NetworkPlayer
{
public:
  NetworkPlayer();
  NetworkPlayer(int id);
  ~NetworkPlayer() = default;

  void setState(shared::PlayerState state);

  void draw(sf::RenderWindow& window);

  int id() const { return _id; }

private:
  int _id;
  shared::PlayerState _state;
  sf::RectangleShape _shape;
};

}