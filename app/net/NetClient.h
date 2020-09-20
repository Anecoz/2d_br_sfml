#pragma once

#include "Network.h"

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

  void hax();

private:
  void processMessages();
  void processMessage(int clientIdx, yojimbo::Message*);
  void processTestMessage(int clientIdx, shared::TestMessage*);

  yojimbo::Client* _client;
  ClientAdapter* _adapter;
  shared::GameConnectionConfig _config;
};

}