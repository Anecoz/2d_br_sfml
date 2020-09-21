#pragma once

#include "Coordinate.h"
#include "Velocity.h"

#include <SFML/Graphics.hpp>

class Player
{
public:
  Player();
  ~Player() = default;

  void handleKeyEvent(sf::Event& keyEvent);
  void update(double dt);
  void draw(sf::RenderWindow& window);

  shared::Coordinate pos() { return _position; }

private:
  const double _speed = 100.0;

  shared::Velocity _velocity;
  shared::Coordinate _position;
  sf::RectangleShape _shape;
};