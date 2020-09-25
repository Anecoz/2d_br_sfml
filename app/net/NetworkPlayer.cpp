#include "NetworkPlayer.h"

#include <iostream>

namespace net {

NetworkPlayer::NetworkPlayer()
  : _id(-1)
  , _shape(sf::RectangleShape(sf::Vector2f(50.0f, 50.0f)))
{
  _shape.setFillColor(sf::Color::Yellow);
}

NetworkPlayer::NetworkPlayer(int id)
  : _id(id)
  , _shape(sf::RectangleShape(sf::Vector2f(50.0f, 50.0f)))
{
  _shape.setFillColor(sf::Color::Yellow);
}

void NetworkPlayer::setState(shared::PlayerState state)
{
  _state = state;
  _shape.setPosition({(float)_state._coord._x, (float)_state._coord._y});
}

void NetworkPlayer::draw(sf::RenderWindow& window)
{
  window.draw(_shape);
}

}