#pragma once

#include "PlayerState.h"
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

  void updateState(shared::PlayerState state);
  shared::PlayerState state() { return _state; }

private:
  const double _speed = 100.0;

  shared::Velocity _velocity;
  shared::PlayerState _state;
  sf::RectangleShape _shape;
  sf::Text _healthText;
};