// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * AiSpecies
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../assets/AssetManager.h"
#include "../ai/AiStateMachine.h"

enum {
  MOVEMENT_NONE,
  MOVEMENT_BALLOON,
  MOVEMENT_HOPPER
};

class AiSpecies {
public:
  // movement types
  int movementType;
//  int groundMoveType;
//  int airMoveType;
//  int waterMoveType;
//  int wallMoveType;

  // attack types
//  int meleeType;
//  int rangedType;

//  int skin;

  // inventory
//  int inventorySize;

  //bool explodeOnDeath;


  AiStateMachine stateMachine;

  bool willAttackSameSpecies;
  //mAttackPlayer
  //mSightRange
};

class SpeciesDataContainer {
private:
  AiSpecies mSpecies[NUM_AITYPES];

  void loadSpeciesData();

public:
  SpeciesDataContainer();

  AiSpecies* get(size_t type);
};

extern SpeciesDataContainer gSpeciesData;
