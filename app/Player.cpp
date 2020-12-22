#include "Player.h"

#include "FontCache.h"

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>

Player::Player()
  : _inputStateChanged(false)
  , _shape(sf::RectangleShape(sf::Vector2f(50.0f, 50.0f)))
{
  _state._health = 100;

  _shape.setFillColor(sf::Color::Blue);
  _shape.setOrigin(25.0f, 25.0f);

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
    if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::A) {
      _velocity._x = 0.0;
    }
    else if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S) {
      _velocity._y = 0.0;
    }
  }
}

void Player::handleMouseEvent(sf::Event& event)
{
  shared::InputState oldState = _inputState;
  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      _inputState._shooting = true;
      std::cout << "Shooting!" << std::endl;
    }
  }
  else if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      _inputState._shooting = false;
      std::cout << "Not Shooting!" << std::endl;
    }
  }
  else if (event.type == sf::Event::MouseMoved) {
    // TODO: Update our rotation based on x/y of mouse. event.mouseMove.x/y are pixel coordinates of mouse
    glm::vec2 up(0.0, -1.0);
    glm::vec2 pToM(
      event.mouseMove.x - _state._coord._x,
      event.mouseMove.y - _state._coord._y);
    pToM = glm::normalize(pToM);
    double angle = glm::degrees(glm::angle(up, pToM));

    // Go from [0, 180] to [0, 360]
    if (event.mouseMove.x < _state._coord._x) {
      angle = 360.0 - angle;
    }
    _state._rotation = angle;
  }

  if (oldState != _inputState) {
    _inputStateChanged = true;
  }
}

void Player::update(double dt)
{
  _state._coord._x = _state._coord._x + _velocity._x * dt;
  _state._coord._y = _state._coord._y + _velocity._y * dt;
  _shape.setPosition(sf::Vector2f((float)_state._coord._x, (float)_state._coord._y));
  _shape.setOrigin(25.0f, 25.0f);
  _shape.setRotation((float)_state._rotation);
}

void Player::updateState(shared::PlayerState state)
{
  // Don't do position now, it'll be weird...
  _state._health = state._health;
  _healthText.setString(std::to_string(_state._health) + " HP");
}

void Player::draw(sf::RenderWindow& window)
{
  window.draw(_healthText);
  window.draw(_shape);
}