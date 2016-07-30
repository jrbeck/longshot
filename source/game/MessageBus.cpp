#include "MessageBus.h"

MessageBus::MessageBus() {
}

MessageBus::~MessageBus() {
}

void MessageBus::sendMessage(const Message& message) {
  mMessages.push_back(message);
}

int MessageBus::getNextMessage(int recipient, Message* message) {
  for (size_t i = 0; i < mMessages.size(); ++i) {
    if (mMessages[i].recipient == recipient) {
      *message = mMessages[i];
      size_t lastMessage = mMessages.size() - 1;

      // get rid of this message
      if (i == lastMessage) {
        mMessages.pop_back();
      }
      else {
        std::swap(mMessages[i], mMessages[lastMessage]);
        mMessages.pop_back();
        i--;
      }
      return 1;
    }
  }

  return 0;
}

void MessageBus::clearMailBox(int recipient) {
  for (size_t i = 0; i < mMessages.size(); ++i) {
    if (mMessages[i].recipient == recipient) {
      size_t lastMessage = mMessages.size() - 1;

      // get rid of this message
      if (i == lastMessage) {
        mMessages.pop_back();
      }
      else {
        std::swap(mMessages[i], mMessages[lastMessage]);
        mMessages.pop_back();
        i--;
      }
    }
  }
}
