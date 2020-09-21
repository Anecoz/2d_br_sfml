#include "Client.h"

#include <utility>

namespace server {

Client::Client(int id)
  : _id(id)
{}

void Client::setCoord(shared::Coordinate&& coord)
{
  _coord = std::move(coord);
}

const shared::Coordinate& Client::getCoord() const
{
  return _coord;
}

}