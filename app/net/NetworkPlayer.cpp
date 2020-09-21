#include "NetworkPlayer.h"

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

void NetworkPlayer::setPosition(shared::Coordinate&& pos)
{
  _position = std::move(pos);
  _shape.setPosition(sf::Vector2f((float)_position._x, (float)_position._y));
}

void NetworkPlayer::draw(sf::RenderWindow& window)
{
  window.draw(_shape);
}

}