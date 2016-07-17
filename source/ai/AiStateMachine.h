// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AiStateMachine
// *
// * this is the data structure for the AI
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include <vector>

using namespace std;


enum {
  AISTATE_ALL,
  AISTATE_STOP,
  AISTATE_WANDER,
  AISTATE_MOVE_TO_WAYPOINT,
  AISTATE_PURSUE,
  AISTATE_ATTACK_MELEE,
  AISTATE_ATTACK_RANGE,
  AISTATE_RANGE,  // i.e. get to a certain range from target
  AISTATE_FLEE,
  AISTATE_FOLLOW,
  AISTATE_FEIGN_DEATH,

  NUM_AISTATES,

  AI_STATE_NONE
};


enum {
  CONDITION_FALSE,  // this is for a state that never changes
  CONDITION_BEGIN_WANDER,
  CONDITION_END_WANDER,
  CONDITION_ENEMY_SPOTTED,
  CONDITION_ATTACKED,
  CONDITION_BADLY_HURT,
  CONDITION_TARGET_IN_RANGE,
  CONDITION_TARGET_TOO_CLOSE,
  CONDITION_TARGET_TOO_FAR,
  CONDITION_TARGET_DEAD,
  CONDITION_TARGET_LOST,

  NUM_CONDITIONS
};



class AiState {
public:
  int state;
  int condition;
  int nextState;
};



class AiStateMachine {
public:
  AiStateMachine();
  ~AiStateMachine();

  void addState(AiState state);

  vector<AiState> mStates;
};

