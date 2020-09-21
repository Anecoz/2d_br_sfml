#pragma once

#include "Coordinate.h"

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

  void setCoord(shared::Coordinate&& coord);
  const shared::Coordinate& getCoord() const;
  int id() {  return _id; }

private:
  int _id;
  shared::Coordinate _coord;  
};

}