#pragma once

#include <yojimbo.h>

namespace shared {

enum class GameMessageType {
  TEST,
  TEST2,
  DISCONNECT,
  POSITION,
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

class Test2Message : public yojimbo::Message
{
public:
  Test2Message()
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

class PositionMessage : public yojimbo::Message
{
public:
  PositionMessage()
    : _x(0.0)
    , _y(0.0)
    , _id(-1)
  {}

  double _x;
  double _y;
  int _id;

  template <typename Stream>
  bool Serialize(Stream& stream) {
    serialize_double(stream, _x);
    serialize_double(stream, _y);
    serialize_int(stream, _id, -1, 64);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class DisconnectMessage : public yojimbo::Message
{
public:
  DisconnectMessage()
    : _id(-1)
  {}

  int _id;

  template <typename Stream>
  bool Serialize(Stream& stream) {
    serialize_int(stream, _id, -1, 64);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)shared::GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::TEST, shared::TestMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::TEST2, shared::Test2Message);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::POSITION, shared::PositionMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::DISCONNECT, shared::DisconnectMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();

}