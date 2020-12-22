#pragma once

#include "Coordinate.h"

namespace shared {

struct InputState
{
  bool operator==(const InputState& other) const {
    return _shooting == other._shooting;
  }

  bool operator!=(const InputState& other) const {
    return !operator==(other);
  }

  bool _shooting = false;
};

struct PlayerState
{
  int _health = -1;
  double _rotation = 0.0;
  Coordinate _coord;
};

}