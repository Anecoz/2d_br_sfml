#pragma once

#include <yojimbo.h>

namespace shared {

enum class GameMessageType {
  TEST,
  COUNT
};

enum class GameChannel {
  RELIABLE,
  UNRELIABLE,
  COUNT
};

struct GameConnectionConfig : yojimbo::ClientServerConfig
{
  GameConnectionConfig()
    : yojimbo::ClientServerConfig()
  {
    numChannels = 2;
    channel[(int)GameChannel::RELIABLE].type = yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
    channel[(int)GameChannel::UNRELIABLE].type = yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
  }
};

class TestMessage : public yojimbo::Message
{
public:
  TestMessage()
    : _data(0)
  {}

  int _data;

  template <typename Stream>
  bool Serialize(Stream& stream) {
    serialize_int(stream, _data, 0, 512);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

}