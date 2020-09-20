#include "NetClient.h"

#include <cstdint>
#include <iostream>
#include <yojimbo.h>

namespace net {

// ------ Yojimbo adapter and factories ---------
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)shared::GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::TEST, shared::TestMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();

class ClientAdapter : public yojimbo::Adapter
{
public:
  explicit ClientAdapter()
  {}

  yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override
  {
    return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
  }
};
// ------ end yojimbo ---------

static const std::uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

NetClient::NetClient(std::string serverAddr)
  : _client(nullptr)
  , _adapter(new ClientAdapter())
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

void NetClient::hax()
{
  shared::TestMessage* msg = (shared::TestMessage*)_client->CreateMessage((int)shared::GameMessageType::TEST);
  msg->_data = 69;
  _client->SendMessage((int)shared::GameChannel::RELIABLE, msg);
}

void NetClient::update(double dt)
{
  _client->AdvanceTime(_client->GetTime() + dt);
  _client->ReceivePackets();

  if (_client->IsConnected()) {
    processMessages();
  }

  _client->SendPackets();
}

void NetClient::processMessages()
{
  for (int i = 0; i < _config.numChannels; ++i) {
    yojimbo::Message* msg = _client->ReceiveMessage(i);
    while (msg != nullptr) {
      processMessage(i, msg);
      _client->ReleaseMessage(msg);
      msg = _client->ReceiveMessage(i);
    }
  }
}

void NetClient::processMessage(int clientIdx, yojimbo::Message* msg)
{
  switch (msg->GetType()) {
    case (int)shared::GameMessageType::TEST:
      processTestMessage(clientIdx, (shared::TestMessage*)msg);
      break;
    default:
      break;
  }
}

void NetClient::processTestMessage(int clientIdx, shared::TestMessage* msg)
{
  std::cout << "CLIENT: got a test message with data: " << msg->_data << std::endl;
}

}