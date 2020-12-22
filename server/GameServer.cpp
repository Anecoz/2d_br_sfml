#include "GameServer.h"

#include <cstdint>
#include <iostream>
#include <yojimbo.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/intersect.hpp>

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
  double fixedDt = 1.0 / 60.0;
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
  updateClientStates();

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
  // Retrieve latest snapshots
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

  // Send any pending welcomes to players
  std::vector<int> localWelcome;
  {
    std::unique_lock<std::mutex> lock(_welcomeMtx);
    std::swap(_queuedWelcome, localWelcome);
  }
  for (auto& idx: localWelcome) {
    for (auto& c: _clients) {
      if (c.id() == idx) {
        shared::WelcomeMessage* msg = (shared::WelcomeMessage*)_yojimboServer->CreateMessage(idx, (int)shared::GameMessageType::WELCOME);
        msg->_id = idx;
        _yojimboServer->SendMessage(idx, (int)shared::GameChannel::RELIABLE, msg);
        break;
      }      
    }
  }

  // Send any pending disconnects to players
  std::vector<int> localDisconnects;
  {
    std::unique_lock<std::mutex> lock(_discQMtx);
    std::swap(_queuedDisconnects, localDisconnects);
  }
  for (auto& idx: localDisconnects) {
    for (auto& c: _clients) {
      shared::DisconnectMessage* msg = (shared::DisconnectMessage*)_yojimboServer->CreateMessage(c.id(), (int)shared::GameMessageType::DISCONNECT);
      msg->_id = idx;
      _yojimboServer->SendMessage(c.id(), (int)shared::GameChannel::RELIABLE, msg);
    }
  }
}

void GameServer::updateClientStates()
{
  // Check if any shooting client is hitting another client.
  for (auto& c: _clients) {
    if (c.inputState()._shooting) {
      // Get the "shooting vector". Take up and rotate by state rotation.
      glm::vec2 up(0.0, -1.0);
      glm::vec2 dir = glm::rotate(up, float(glm::radians(c.state()._rotation)));
      glm::vec2 orig(c.state()._coord._x, c.state()._coord._y);

      for (auto& other: _clients) {
        if (other.id() == c.id()) continue;

        glm::vec2 otherCenter(other.state()._coord._x, other.state()._coord._y);
        glm::vec2 iPos, iNormal;
        if (glm::intersectRaySphere(orig, dir, otherCenter, 25.0f, iPos, iNormal)) {
          std::cout << "Holy moly! We hit something!" << std::endl;
          auto state = other.state();
          state._health = state._health - 1.0;
          other.setState(state);
          break;
        }
      }
    }
  }

  // Send latest snapshots to connected clients.
  for (auto& sender: _clients) {
    for (auto& c: _clients) {
      // We need to send the state update to the own player aswell.
      shared::PlayerStateMessage* msg = (shared::PlayerStateMessage*)_yojimboServer->CreateMessage(c.id(), (int)shared::GameMessageType::PLAYER_STATE);
      msg->_id = sender.id();
      msg->_state = sender.state();
      _yojimboServer->SendMessage(c.id(), (int)shared::GameChannel::UNRELIABLE, msg);
    }
  }
}

void GameServer::processMessage(int clientIdx, yojimbo::Message* msg)
{
  switch (msg->GetType()) {
    case (int)shared::GameMessageType::INPUT_STATE:
      processInputMessage(clientIdx, (shared::InputStateMessage*)msg);
      break;
    case (int)shared::GameMessageType::PLAYER_STATE:
      processStateMessage(clientIdx, (shared::PlayerStateMessage*)msg);
      break;
    default:
      break;
  }
}

void GameServer::processStateMessage(int clientIdx, shared::PlayerStateMessage* msg)
{
  auto client = getClient(clientIdx);
  if (!client) {
    std::cerr << "Received state update for client that doesn't exist." << std::endl;
    return;
  }

  // We have authority over stuff like health, so ignore that.
  auto state = client->state();
  msg->_state._health = state._health;
  client->setState(msg->_state);
}

void GameServer::processInputMessage(int clientIdx, shared::InputStateMessage* msg)
{
  auto client = getClient(clientIdx);
  if (!client) {
    std::cerr << "Received input state update for client that doesn't exist." << std::endl;
    return;
  }

  client->setInputState(msg->_state);
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

  std::unique_lock<std::mutex> lock(_welcomeMtx);
  _queuedWelcome.emplace_back(idx);
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
  else {
    std::unique_lock<std::mutex> lock(_discQMtx);
    _queuedDisconnects.emplace_back(idx);
  }

  std::cout << "Client " << idx << " disconnected" << std::endl;
}

}