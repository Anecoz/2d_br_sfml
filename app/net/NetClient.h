#pragma once

#include "Network.h"
#include "PlayerState.h"
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
  void queueStateUpdate(shared::PlayerState state);

  bool localStateUpdated() { return _localStateQueued; }
  shared::PlayerState getUpdatedLocalState();

  void drawNetPlayers(sf::RenderWindow& window);

private:
  void processMessages();
  void processMessage(yojimbo::Message*);
  void processStateUpdate(shared::PlayerStateMessage*);
  void processDisconnect(shared::DisconnectMessage*);
  void processWelcome(shared::WelcomeMessage*);

  yojimbo::Client* _client;
  ClientAdapter* _adapter;
  shared::GameConnectionConfig _config;

  bool _stateQueued;
  shared::PlayerState _queuedState;

  bool _localStateQueued;
  shared::PlayerState _queuedLocalState;

  std::vector<NetworkPlayer> _netPlayers;
  int _localId;
};

}