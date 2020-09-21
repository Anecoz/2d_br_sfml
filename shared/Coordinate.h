#pragma once

namespace shared {

struct Coordinate
{
  Coordinate() : _x(0.0), _y(0.0) {}
  Coordinate(double x, double y) : _x(x), _y(y) {}

  double _x;
  double _y;
};

}