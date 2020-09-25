#pragma once

#include "Client.h"

#include "Network.h"

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

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

  void processStateMessage(int clientIdx, shared::PlayerStateMessage*);

  Client* getClient(int id);

  std::vector<Client> _clients;

  std::mutex _discQMtx;
  std::vector<int> _queuedDisconnects;

  std::mutex _welcomeMtx;
  std::vector<int> _queuedWelcome;

  bool _running;
  GameAdapter* _adapter;
  yojimbo::Server* _yojimboServer;
  shared::GameConnectionConfig _config;
  double _time;
};

}