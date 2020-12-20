#pragma once

#include "PlayerState.h"

namespace server {

class Client
{
public:
  Client() : _id(-1) {}
  Client(int id);
  ~Client() = default;

  explicit operator bool() const {
    return _id != -1;
  }

  void setState(shared::PlayerState state);
  shared::PlayerState state() const;

  void setInputState(shared::InputState state);
  shared::InputState inputState() const;

  int id() const { return _id; }

private:
  int _id;
  shared::PlayerState _state;
  shared::InputState _inputState;
};

}