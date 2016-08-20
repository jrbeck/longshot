#include "../player/Player.h"
#include "../player/PlayerView.h"

class PlayerController {
public:
  PlayerController(GameModel* gameModel, PlayerView* playerView);
  ~PlayerController();

  void update(GameInput* gameInput);

  MovementInput mMovementInput;

private:
  void handleInventoryInput();
  void handleItemUsage();
  void handleDeadPlayerInput();
  void updateMovementInput();

  GameInput* mGameInput;
  GameModel* mGameModel;
  Player* mPlayer;
  PlayerView* mPlayerView;
};
