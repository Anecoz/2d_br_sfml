#include "Player.h"

Player::Player()
  : _shape(sf::RectangleShape(sf::Vector2f(50.0f, 50.0f)))
{
  _shape.setFillColor(sf::Color::Blue);
}

void Player::handleKeyEvent(sf::Event& event)
{
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::D) {
      _velocity._x = _speed;
    }
    else if (event.key.code == sf::Keyboard::A) {
      _velocity._x = -_speed;
    }
    else if (event.key.code == sf::Keyboard::W) {
      _velocity._y = -_speed;
    }
    else if (event.key.code == sf::Keyboard::S) {
      _velocity._y = _speed;
    }
  }
  else if (event.type == sf::Event::KeyReleased) {
    if (event.key.code == sf::Keyboard::D) {
      _velocity._x = 0.0;
    }
    else if (event.key.code == sf::Keyboard::A) {
      _velocity._x = 0.0;
    }
    else if (event.key.code == sf::Keyboard::W) {
      _velocity._y = 0.0;
    }
    else if (event.key.code == sf::Keyboard::S) {
      _velocity._y = 0.0;
    }
  }
}

void Player::update(double dt)
{
  _position._x = _position._x + _velocity._x * dt;
  _position._y = _position._y + _velocity._y * dt;
  _shape.setPosition(sf::Vector2f((float)_position._x, (float)_position._y));
}

void Player::draw(sf::RenderWindow& window)
{
  window.draw(_shape);
}