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
  void handleMouseEvent(sf::Event& mouseEvent);

  void update(double dt, int mX, int mY);
  void draw(sf::RenderWindow& window);

  void updateState(shared::PlayerState state);
  shared::PlayerState state() { return _state; }

  bool inputStateChanged() const { return _inputStateChanged; }
  shared::InputState inputState() { _inputStateChanged = false; return _inputState; }

private:
  const double _speed = 500.0;

  shared::Velocity _velocity;
  shared::PlayerState _state;
  shared::InputState _inputState;
  bool _inputStateChanged;
  sf::RectangleShape _shape;
  sf::Text _healthText;
};