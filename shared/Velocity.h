#pragma once

namespace shared {

struct Velocity
{
  Velocity() : _x(0.0), _y(0.0) {}
  Velocity(double x, double y) : _x(x), _y(y) {}

  double _x;
  double _y;
};

}