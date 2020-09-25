#include "Player.h"

#include "FontCache.h"

Player::Player()
  : _shape(sf::RectangleShape(sf::Vector2f(50.0f, 50.0f)))
{
  _state._health = 100;

  _shape.setFillColor(sf::Color::Blue);

  _healthText.setFont(fontcache::font());
  _healthText.setCharacterSize(24);
  _healthText.setString("100 HP");
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
  _state._coord._x = _state._coord._x + _velocity._x * dt;
  _state._coord._y = _state._coord._y + _velocity._y * dt;
  _shape.setPosition(sf::Vector2f((float)_state._coord._x, (float)_state._coord._y));
}

void Player::updateState(shared::PlayerState state)
{
  // Don't do position now, it'll be weird...
  _state._health = state._health;
  _healthText.setString(std::to_string(_state._health) + " HP");
}

void Player::draw(sf::RenderWindow& window)
{
  window.draw(_shape);
  window.draw(_healthText);
}