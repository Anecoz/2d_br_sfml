#include "GameServer.h"

#include <cstdint>
#include <iostream>
#include <yojimbo.h>

namespace server {

// ------ Yojimbo adapter and factories ---------
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)shared::GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::TEST, shared::TestMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();

class GameAdapter : public yojimbo::Adapter
{
public:
  explicit GameAdapter(GameServer* server = nullptr)
    : _server(server)
  {}

  yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override
  {
    return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
  }

  void OnServerClientConnected(int clientIndex) override
  {
    if (_server != nullptr) {
      // client connected, tell our actual server implementation!
      _server->clientConnected(clientIndex);
    }
  }

  void OnServerClientDisconnected(int clientIndex) override
  {
    if (_server != nullptr) {
      // client disconnected, tell our actual server implementation!
      _server->clientDisconnected(clientIndex);
    }
  }  

private:
  GameServer* _server;
};
// ------ end yojimbo ---------

static const std::uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };
static const int MAX_PLAYERS = 64;

GameServer::GameServer(std::string address)
  : _running(false)
  , _adapter(nullptr)
  , _yojimboServer(nullptr)
  , _config()
{
  InitializeYojimbo();

  yojimbo::Address yAddr(address.c_str());
  _adapter = new GameAdapter(this);
  _yojimboServer = new yojimbo::Server(yojimbo::GetDefaultAllocator(), DEFAULT_PRIVATE_KEY, yAddr, _config, *_adapter, 0.0);
}

GameServer::~GameServer()
{
  _running = false;
  _yojimboServer->Stop();
  delete _adapter;
  delete _yojimboServer;
  ShutdownYojimbo();
}

bool GameServer::start()
{
  _yojimboServer->Start(MAX_PLAYERS);
  if (!_yojimboServer->IsRunning()) {
    return false;
  }

  char buffer[256];
  _yojimboServer->GetAddress().ToString(buffer, sizeof(buffer));
  std::cout << "Server started on address: " << buffer << std::endl;
  _running = true;

  return true;
}

void GameServer::run()
{
  double time = 0.0;
  double fixedDt = 1.0 / 60.0;
  while (_running) {
    double currentTime = yojimbo_time();
    if (time <= currentTime) {
      update(fixedDt);
      time += fixedDt;
    }
    else {
      yojimbo_sleep(time - currentTime);
    }
  }
}

void GameServer::update(double dt)
{
  if (!_yojimboServer->IsRunning()) {
    _running = false;
    return;
  }

  _yojimboServer->AdvanceTime(dt);
  _yojimboServer->ReceivePackets();
  processMessages();

  _yojimboServer->SendPackets();
}

void GameServer::processMessages()
{
  for (int i = 0; i < MAX_PLAYERS; ++i) {
    if (_yojimboServer->IsClientConnected(i)) {
      for (int j = 0; j < _config.numChannels; ++j) {
        yojimbo::Message* msg = _yojimboServer->ReceiveMessage(i, j);
        while (msg != nullptr) {
          processMessage(i, msg);
          _yojimboServer->ReleaseMessage(i, msg);
          msg = _yojimboServer->ReceiveMessage(i, j);
        }
      }
    }
  }
}

void GameServer::processMessage(int clientIdx, yojimbo::Message* msg)
{
  switch (msg->GetType()) {
    case (int)shared::GameMessageType::TEST:
      processTestMessage(clientIdx, (shared::TestMessage*)msg);
      break;
    default:
      break;
  }
}

void GameServer::processTestMessage(int clientIdx, shared::TestMessage* msg)
{
  std::cout << "SERVER: got a test message with data: " << msg->_data << std::endl;
}

void GameServer::clientConnected(int idx)
{
  std::cout << "Client " << idx << " connected" << std::endl;
}

void GameServer::clientDisconnected(int idx)
{
  std::cout << "Client " << idx << " disconnected" << std::endl;
}

}