#include "../player/Player.h"

struct MovementInput {
  v2d_t walkInput;
  double facingDelta;
  double inclinationDelta;
  bool isJumping;
  bool isSwimming;
};

class PlayerController {
public:
  PlayerController(GameModel* gameModel);
  ~PlayerController();

  void update(GameInput* gameInput);

  // MovementInput* getMovementInput();

  void applyMovementInput();
  void checkForBlockDamage();

private:
  void handleItemUsage();
  void handleDeadPlayerInput();
  void updateMovementInput();

  void applyMovementInputDry(PhysicsEntity* physicsEntity);
  void applyMovementInputWet(PhysicsEntity* physicsEntity);

  GameInput* mGameInput;
  GameModel* mGameModel;
  Player* mPlayer;
  MovementInput mMovementInput;
};
