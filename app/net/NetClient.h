#pragma once

#include "Network.h"
#include "Coordinate.h"
#include "NetworkPlayer.h"

#include <string>
#include <vector>

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

  void drawNetPlayers(sf::RenderWindow& window);

private:
  void processMessages();
  void processMessage(yojimbo::Message*);
  void processPositionUpdate(shared::PositionMessage*);
  void processDisconnect(shared::DisconnectMessage*);

  yojimbo::Client* _client;
  ClientAdapter* _adapter;
  shared::GameConnectionConfig _config;
  bool _coordinateQueued;
  shared::Coordinate _queuedCoordinate;

  std::vector<NetworkPlayer> _netPlayers;
};

}