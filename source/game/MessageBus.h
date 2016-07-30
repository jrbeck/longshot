#pragma once

#include <algorithm>
#include <vector>

#include "../math/v3d.h"

typedef struct {
  int sender;
  int recipient;

  int type;

  int iValue;
  int iValue2;

  double dValue;

  v3d_t vec3;
} Message;

enum {
  MAILBOX_PHYSICS,
  MAILBOX_ITEM_MANAGER,
  MAILBOX_AI_MANAGER
};

enum {
  MESSAGE_DAMAGE,
  MESSAGE_ITEMGRAB,
  MESSAGE_ITEM_DESTROYED,
  MESSAGE_SPAWN_CREATURE,
  MESSAGE_PLAYERPICKUPITEMS,
  MESSAGE_ITEMGRABBED,
  MESSAGE_ADJUSTHEALTH,

  NUM_MESSSAGE_TYPES
};

class MessageBus {
public:
  MessageBus();
  ~MessageBus();

  void sendMessage(const Message& message);
  int getNextMessage(int recipient, Message* message);
  void clearMailBox(int recipient);

private:
  std::vector<Message> mMessages;
};
