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
  , _stateQueued(false)
  , _inputQueued(false)
  , _localStateQueued(false)
  , _localId(-1)
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

void NetClient::queueStateUpdate(shared::PlayerState state)
{
  _stateQueued = true;
  _queuedState = state;
}

void NetClient::queueInputUpdate(shared::InputState state)
{
  _inputQueued = true;
  _queuedInputState = state;
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
    if (_stateQueued) {
      _stateQueued = false;
      shared::PlayerStateMessage* stateMsg = (shared::PlayerStateMessage*)_client->CreateMessage((int)shared::GameMessageType::PLAYER_STATE);
      stateMsg->_state = _queuedState;
      _client->SendMessage((int)shared::GameChannel::UNRELIABLE, stateMsg);
    }
    if (_inputQueued) {
      _inputQueued = false;
      shared::InputStateMessage* inputMsg = (shared::InputStateMessage*)_client->CreateMessage((int)shared::GameMessageType::INPUT_STATE);
      inputMsg->_state = _queuedInputState;
      _client->SendMessage((int)shared::GameChannel::RELIABLE, inputMsg);
    }
  }

  _client->SendPackets();
}

shared::PlayerState NetClient::getUpdatedLocalState()
{
  _localStateQueued = false;
  return _queuedLocalState;
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
    case (int)shared::GameMessageType::DISCONNECT:
      processDisconnect((shared::DisconnectMessage*)msg);
      break;
    case (int)shared::GameMessageType::WELCOME:
      processWelcome((shared::WelcomeMessage*)msg);
      break;
    case (int)shared::GameMessageType::PLAYER_STATE:
      processStateUpdate((shared::PlayerStateMessage*)msg);
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

void NetClient::processWelcome(shared::WelcomeMessage* msg)
{
  std::cout << "Received welcome, our id is: " << std::to_string(msg->_id) << std::endl;
  _localId = msg->_id;
}

void NetClient::processStateUpdate(shared::PlayerStateMessage* msg)
{
  int id = msg->_id;

  // Are we updating our own local player?
  if (id == _localId) {
    _localStateQueued = true;
    _queuedLocalState = msg->_state;
    return;
  }

  // Do we have this player already?
  auto it = std::find_if(_netPlayers.begin(), _netPlayers.end(), [id](const NetworkPlayer& player) { return player.id() == id; });
  net::NetworkPlayer* player = nullptr;
  if (it != _netPlayers.end()) {
    player = &(*it);
  }
  else {
    // Doesn't exist, create it
    _netPlayers.emplace_back(id);
    player = &_netPlayers.back();
  }

  if (player != nullptr) {
    player->setState(msg->_state);
  }
}

}