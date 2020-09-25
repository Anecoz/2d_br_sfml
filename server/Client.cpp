#include "Client.h"

#include <utility>

namespace server {

Client::Client(int id)
  : _id(id)
{
  _state._health = 100;
}

void Client::setState(shared::PlayerState state)
{
  _state = state;
}

shared::PlayerState Client::state() const
{
  return _state;
}

}