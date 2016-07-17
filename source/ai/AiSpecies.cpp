#include "AiSpecies.h"

SpeciesDataContainer gSpeciesData;

SpeciesDataContainer::SpeciesDataContainer() {
  loadSpeciesData();
}

void SpeciesDataContainer::loadSpeciesData() {
  AiState state;

//  AITYPE_DUMMY
  mSpecies[AITYPE_DUMMY].movementType = MOVEMENT_NONE;
  mSpecies[AITYPE_DUMMY].willAttackSameSpecies = false;

  state.state = AISTATE_STOP;
  state.condition = CONDITION_FALSE;
  state.nextState = AISTATE_STOP;
  mSpecies[AITYPE_DUMMY].stateMachine.addState(state);

//  AITYPE_BALLOON
  mSpecies[AITYPE_BALLOON].movementType = MOVEMENT_BALLOON;
  mSpecies[AITYPE_BALLOON].willAttackSameSpecies = true;

/*  state.state = AISTATE_ALL;
  state.condition = CONDITION_BADLY_HURT;
  state.nextState = AISTATE_FLEE;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);
*/

  state.state = AISTATE_STOP;
  state.condition = CONDITION_ENEMY_SPOTTED;
  state.nextState = AISTATE_RANGE;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_STOP;
  state.condition = CONDITION_BEGIN_WANDER;
  state.nextState = AISTATE_WANDER;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_WANDER;
  state.condition = CONDITION_END_WANDER;
  state.nextState = AISTATE_STOP;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_RANGE;
  state.condition = CONDITION_TARGET_IN_RANGE;
  state.nextState = AISTATE_ATTACK_RANGE;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_RANGE;
  state.condition = CONDITION_TARGET_DEAD;
  state.nextState = AISTATE_STOP;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_RANGE;
  state.condition = CONDITION_TARGET_LOST;
  state.nextState = AISTATE_STOP;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_ATTACK_RANGE;
  state.condition = CONDITION_TARGET_TOO_CLOSE;
  state.nextState = AISTATE_ATTACK_RANGE;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_ATTACK_RANGE;
  state.condition = CONDITION_TARGET_TOO_FAR;
  state.nextState = AISTATE_ATTACK_RANGE;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_ATTACK_RANGE;
  state.condition = CONDITION_TARGET_DEAD;
  state.nextState = AISTATE_STOP;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

  state.state = AISTATE_ATTACK_RANGE;
  state.condition = CONDITION_TARGET_LOST;
  state.nextState = AISTATE_STOP;
  mSpecies[AITYPE_BALLOON].stateMachine.addState(state);

//  AITYPE_SHOOTER
  mSpecies[AITYPE_SHOOTER].movementType = MOVEMENT_BALLOON;
  mSpecies[AITYPE_SHOOTER].willAttackSameSpecies = false;

//  AITYPE_HOPPER
  mSpecies[AITYPE_HOPPER].movementType = MOVEMENT_HOPPER;
  mSpecies[AITYPE_HOPPER].willAttackSameSpecies = false;

//  AITYPE_HUMAN
  mSpecies[AITYPE_HUMAN].movementType = MOVEMENT_HOPPER;
  mSpecies[AITYPE_HUMAN].willAttackSameSpecies = false;
}

AiSpecies* SpeciesDataContainer::get(size_t type) {
  if (type < NUM_AITYPES) {
    return &mSpecies[type];
  }

  return NULL;
}
