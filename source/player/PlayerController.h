#include "../player/Player.h"

class PlayerController {
public:
  PlayerController(GameModel* gameModel);
  ~PlayerController();

  void update(GameInput* gameInput);

  MovementInput* getMovementInput();

private:
  void handleItemUsage();
  void handleDeadPlayerInput();
  void updateMovementInput();

  GameInput* mGameInput;
  GameModel* mGameModel;
  Player* mPlayer;
  MovementInput mMovementInput;
};
