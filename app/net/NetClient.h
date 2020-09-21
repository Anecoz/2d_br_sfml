#pragma once

#include "Network.h"
#include "Coordinate.h"

#include <string>

namespace yojimbo {class Client;}

namespace net {

class ClientAdapter;

class NetClient
{
public:
  NetClient(std::string serverAddr);
  ~NetClient();

  void update(double dt);
  void queuePositionUpdate(shared::Coordinate&& coord);

private:
  void processMessages();
  void processMessage(int clientIdx, yojimbo::Message*);
  void processTestMessage(int clientIdx, shared::TestMessage*);

  yojimbo::Client* _client;
  ClientAdapter* _adapter;
  shared::GameConnectionConfig _config;
  bool _coordinateQueued;
  shared::Coordinate _queuedCoordinate;
};

}