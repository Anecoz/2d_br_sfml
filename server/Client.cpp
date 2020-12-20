#include "Client.h"

#include <string>
#include <iostream>

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

void Client::setInputState(shared::InputState state)
{
  if (state._shooting != _inputState._shooting) {
    if (state._shooting) {
      std::cout << "Client " << std::to_string(_id) << " is now shooting!" << std::endl;
    }
    else {
      std::cout << "Client " << std::to_string(_id) << " is no longer shooting!" << std::endl;
    }
  }
  _inputState = state;
}

shared::InputState Client::inputState() const
{
  return _inputState;
}

}