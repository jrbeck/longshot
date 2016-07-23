#include "../ai/AiStateMachine.h"

AiStateMachine::AiStateMachine() {
}

AiStateMachine::~AiStateMachine() {
}

void AiStateMachine::addState(AiState state) {
  for (size_t i = 0; i < mStates.size(); ++i) {
    // prevent duplicates
    if (mStates[i].state == state.state &&
      mStates[i].condition == state.condition &&
      mStates[i].nextState == state.nextState)
    {
      return;
    }
  }

  mStates.push_back(state);
}
