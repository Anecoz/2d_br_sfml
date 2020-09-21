#pragma once

#include "Coordinate.h"

#include <SFML/Graphics.hpp>

namespace net {

class NetworkPlayer
{
public:
  NetworkPlayer();
  NetworkPlayer(int id);
  ~NetworkPlayer() = default;

  void setPosition(shared::Coordinate&& pos);
  void draw(sf::RenderWindow& window);

  int id() const { return _id; }

private:
  int _id;
  shared::Coordinate _position;
  sf::RectangleShape _shape;
};

}