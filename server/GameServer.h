#pragma once

#include "Network.h"

#include <cstdint>
#include <string>

namespace yojimbo { class Server; class Message; }

namespace server {

class GameAdapter;

class GameServer
{
public:
  GameServer(std::string address);
  ~GameServer();

  bool start();
  void run();

  void clientConnected(int idx);
  void clientDisconnected(int idx);

private:
  void update(double dt);
  void processMessages();
  void processMessage(int clientIdx, yojimbo::Message*);
  void processTestMessage(int clientIdx, shared::TestMessage*);

  bool _running;
  GameAdapter* _adapter;
  yojimbo::Server* _yojimboServer;
  shared::GameConnectionConfig _config;
};

}