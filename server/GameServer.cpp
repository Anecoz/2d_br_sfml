#include "GameServer.h"

#include <cstdint>
#include <iostream>
#include <yojimbo.h>

namespace server {

// ------ Yojimbo adapter ---------
class GameAdapter : public yojimbo::Adapter
{
public:
  explicit GameAdapter(GameServer* server = nullptr)
    : _server(server)
  {}

  yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override
  {
    return YOJIMBO_NEW(allocator, shared::GameMessageFactory, allocator);
  }

  void OnServerClientConnected(int clientIndex) override
  {
    if (_server != nullptr) {
      _server->clientConnected(clientIndex);
    }
  }

  void OnServerClientDisconnected(int clientIndex) override
  {
    if (_server != nullptr) {
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
  , _time(0.0)
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
  double fixedDt = 1.0 / 128.0;
  while (_running) {
    double currentTime = yojimbo_time();
    if (_time <= currentTime) {
      update(fixedDt);
      _time += fixedDt;
    }
    else {
      yojimbo_sleep(_time - currentTime);
    }
  }
}

void GameServer::update(double dt)
{
  if (!_yojimboServer->IsRunning()) {
    _running = false;
    return;
  }

  _yojimboServer->AdvanceTime(_time);
  _yojimboServer->ReceivePackets();
  processMessages();

  _yojimboServer->SendPackets();
}

Client* GameServer::getClient(int id)
{
  Client* ret = nullptr;
  for (auto& c : _clients) {
    if (c.id() == id) {
      ret = &c;
      break;
    }
  }
  return ret;
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
    case (int)shared::GameMessageType::TEST2:
      processTest2Message(clientIdx, (shared::Test2Message*)msg);
      break;
    case (int)shared::GameMessageType::POSITION:
      processPositionMessage(clientIdx, (shared::PositionMessage*)msg);
      break;
    default:
      break;
  }
}

void GameServer::processTestMessage(int clientIdx, shared::TestMessage* msg)
{
  std::cout << "SERVER: got a test message with data: " << msg->_data << std::endl;
}

void GameServer::processPositionMessage(int clientIdx, shared::PositionMessage* msg)
{
  auto client = getClient(clientIdx);
  if (!client) {
    std::cerr << "Received position update for client that doesn't exist." << std::endl;
    return;
  }

  client->setCoord({msg->_x, msg->_y});
  std::cout << "Updated coord of client " << std::to_string(clientIdx) << " to " << std::to_string(msg->_x) << ", " << std::to_string(msg->_y) << std::endl;
}

void GameServer::processTest2Message(int clientIdx, shared::Test2Message* msg)
{
  std::cout << "SERVER: got a test2 message with data: " << msg->_data << std::endl;
}

void GameServer::clientConnected(int idx)
{
  if (getClient(idx) != nullptr) {
    std::cerr << "SERVER: A client connected " << "(" << idx << ") " << ", but we have a client on that idx already! Disregarding new client." << std::endl;
    return;
  }

  _clients.emplace_back(idx);
  std::cout << "Client " << idx << " connected" << std::endl;
  std::cout << "We now have " << std::to_string(_clients.size()) << " client(s)." << std::endl;
}

void GameServer::clientDisconnected(int idx)
{
  bool erased = false;
  for (auto it = _clients.begin(); it != _clients.end(); ++it) {
    if (it->id() == idx) {
      _clients.erase(it);
      erased = true;
      break;
    }
  }

  if (!erased) {
    std::cerr << "SERVER: A client disconnected " << "(" << idx << ") " << ", but we don't have a client on that index." << std::endl;
    return;
  }

  std::cout << "Client " << idx << " disconnected" << std::endl;
}

}