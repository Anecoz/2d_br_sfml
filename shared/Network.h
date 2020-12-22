#pragma once

#include "PlayerState.h"

#include <yojimbo.h>

namespace shared {

enum class GameMessageType {
  WELCOME,
  DISCONNECT,
  INPUT_STATE,
  PLAYER_STATE,
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
    channel[(int)GameChannel::RELIABLE].messageSendQueueSize = 2048;

    channel[(int)GameChannel::UNRELIABLE].type = yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
  }
};

class WelcomeMessage : public yojimbo::Message
{
public:
  WelcomeMessage()
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

class PlayerStateMessage : public yojimbo::Message
{
public:
  PlayerStateMessage()
    : _id(-1)
  {}

  int _id;
  shared::PlayerState _state;

  template <typename Stream>
  bool Serialize(Stream& stream) {
    serialize_double(stream, _state._coord._x);
    serialize_double(stream, _state._coord._y);
    serialize_double(stream, _state._rotation);
    serialize_int(stream, _id, -1, 64);
    serialize_int(stream, _state._health, -1, 100);
    return true;
  }

  YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class InputStateMessage : public yojimbo::Message
{
public:
  InputStateMessage()
    : _id(-1)
  {}

  int _id;
  shared::InputState _state;

  template <typename Stream>
  bool Serialize(Stream& stream) {
    serialize_int(stream, _id, -1, 64);
    serialize_bool(stream, _state._shooting);
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
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::WELCOME, shared::WelcomeMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::INPUT_STATE, shared::InputStateMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::PLAYER_STATE, shared::PlayerStateMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)shared::GameMessageType::DISCONNECT, shared::DisconnectMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();

}