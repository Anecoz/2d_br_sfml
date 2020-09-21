#include "NetClient.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <yojimbo.h>

namespace net {

// ------ Yojimbo adapter ---------
class ClientAdapter : public yojimbo::Adapter
{
public:
  explicit ClientAdapter()
  {}

  yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override
  {
    return YOJIMBO_NEW(allocator, shared::GameMessageFactory, allocator);
  }
};
// ------ end yojimbo ---------

static const std::uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

NetClient::NetClient(std::string serverAddr)
  : _client(nullptr)
  , _adapter(new ClientAdapter())
  , _coordinateQueued(false)
{
  InitializeYojimbo();

  _client = new yojimbo::Client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"), _config, *_adapter, 0.0);

  std::cout << "Client attempting to connect to " << serverAddr << std::endl;

  std::uint64_t clientId;
  yojimbo::Address addr(serverAddr.c_str());
  yojimbo::random_bytes((uint8_t*)&clientId, 8);
  _client->InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, addr);
}

NetClient::~NetClient()
{
  _client->Disconnect();
  delete _client;
  delete _adapter;
  ShutdownYojimbo();
}

void NetClient::queuePositionUpdate(shared::Coordinate&& coord)
{
  _coordinateQueued = true;
  _queuedCoordinate = std::move(coord);
}

void NetClient::drawNetPlayers(sf::RenderWindow& window)
{
  for (auto& p: _netPlayers) {
    p.draw(window);
  }
}

void NetClient::update(double dt)
{
  _client->AdvanceTime(_client->GetTime() + dt);
  _client->ReceivePackets();

  if (_client->IsConnected()) {
    processMessages();

    // Send queued up packages here!
    if (_coordinateQueued) {
      _coordinateQueued = false;
      shared::PositionMessage* msg = (shared::PositionMessage*)_client->CreateMessage((int)shared::GameMessageType::POSITION);
      msg->_x = _queuedCoordinate._x;
      msg->_y = _queuedCoordinate._y;
      _client->SendMessage((int)shared::GameChannel::UNRELIABLE, msg);
    }
  }

  _client->SendPackets();
}

void NetClient::processMessages()
{
  for (int i = 0; i < _config.numChannels; ++i) {
    yojimbo::Message* msg = _client->ReceiveMessage(i);
    while (msg != nullptr) {
      processMessage(msg);
      _client->ReleaseMessage(msg);
      msg = _client->ReceiveMessage(i);
    }
  }
}

void NetClient::processMessage(yojimbo::Message* msg)
{
  switch (msg->GetType()) {
    case (int)shared::GameMessageType::TEST:      
      break;
    case (int)shared::GameMessageType::TEST2:
      break;
    case (int)shared::GameMessageType::DISCONNECT:
      processDisconnect((shared::DisconnectMessage*)msg);
      break;
    case (int)shared::GameMessageType::POSITION:
      processPositionUpdate((shared::PositionMessage*)msg);
      break;
    default:
      break;
  }
}

void NetClient::processDisconnect(shared::DisconnectMessage* msg)
{
  int id = msg->_id;
  auto it = std::find_if(_netPlayers.begin(), _netPlayers.end(), [id](const NetworkPlayer& player) { return player.id() == id; });
  if (it != _netPlayers.end()) {
    _netPlayers.erase(it);
  }
}

void NetClient::processPositionUpdate(shared::PositionMessage* msg)
{
  int id = msg->_id;

  // Do we have this player already?
  auto it = std::find_if(_netPlayers.begin(), _netPlayers.end(), [id](const NetworkPlayer& player) { return player.id() == id; });
  if (it != _netPlayers.end()) {
    it->setPosition({msg->_x, msg->_y });
  }
  else {
    // Doesn't exist, create it
    _netPlayers.emplace_back(id);
    _netPlayers.back().setPosition({msg->_x, msg->_y});
  }
}

}