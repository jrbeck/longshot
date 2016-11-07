#include "Longshot.h"

Longshot::Longshot() :
  mMainMenu(NULL),
  mGame(NULL),
  mRogueViewer(NULL)
{
  printf("Longshot constructor ----------------\n");

  // lets hope this stays this way
  mConstructorSuccessful = true;

  mGameWindow = new GameWindow("Longshot");
  // TODO: here would be a good place to do something like:
  // mGameWindow->getConstructorSuccess() ...
  // just sayin'...
  mGameWindow->setIcon("art/resource/longshot_icon.png");

  SDL_ShowCursor(1);

  // GameMenu * * * * * * * * * * * * * * * * * * * * * * * *
  reloadMenu();

  // start off in the menu
  mProgramMode = PROGRAM_MODE_MENU;

  FileSystem::createFolder(SAVE_FOLDER);

  printf("%6d: exiting Longshot constructor\n", SDL_GetTicks());
  printf("-----------------------------------\n\n");
}

Longshot::~Longshot() {
  printf("%6d: Longshot destructor\n", SDL_GetTicks());
  printf("---------------------------------------------------------\n");

  if (mMainMenu != NULL) {
    delete mMainMenu;
    mMainMenu = NULL;
  }

  // this should never happen
  if (mGame != NULL) {
    printf("Longshot::~Longshot(): error: mGame not NULL\n");
    delete mGame;
    mGame = NULL;
  }

  if (gDefaultFontTextureHandle != 0) {
    glDeleteTextures(1, &gDefaultFontTextureHandle);
  }

  if (mGameWindow != NULL) {
    delete mGameWindow;
    mGameWindow = NULL;
  }

  printf("\n");
  printf("---------------------------------------------------------\n");
  printf("program execution time: %fs\n", (double)SDL_GetTicks() / 1000.0);
}

void Longshot::reloadMenu() {
  // WARNING: this is where the default font is being loaded
  // it has to be done when there is a context change (e.g.
  // windowed mode->fullscreen), so it is here...
  if (gDefaultFontTextureHandle != 0) {
    glDeleteTextures(1, &gDefaultFontTextureHandle);
  }
  if (AssetManager::loadTexture("art/fonts/font3.png", &gDefaultFontTextureHandle) != 0) {
    printf("Longshot::reloadMenu(): failed to load font\n");
  }
  GameMenu::setDefaultTextureHandle(gDefaultFontTextureHandle);

  // create a new menu
  if (mMainMenu != NULL) {
    delete mMainMenu;
    mMainMenu = NULL;
  }
  mMainMenu = new GameMenu();

  GLfloat color[4] = { 0.55f, 0.075f, 0.075f, 1.0f };
  GLfloat bgColor[4] = { 0.2f, 0.5f, 0.5f, 1.0f };
  v2d_t fontSize = { 0.03f, 0.06f };

  mMainMenu->addText(v2d_v(0.6, 0.0), v2d_v(0.2, 0.2), fontSize, "longshot", TEXT_JUSTIFICATION_RIGHT, color, NULL);

//  GLfloat color2[4] = { 0.2f, 0.0f, 0.0f, 1.0f };
  GLfloat color2[4] = { 0.8f, 0.4f, 0.0f, 1.0f };
  GLfloat bgColor2[4] = { 0.55f, 0.075f, 0.075f, 1.0f };

  fontSize = v2d_v(0.03f, 0.06f);

  mMainMenu->addButton(v2d_v(0.5, 0.35), v2d_v(0.3, 0.1), fontSize, "new game", TEXT_JUSTIFICATION_RIGHT, PROGRAM_MODE_NEW_GAME, color2, bgColor2);
  mMainMenu->addButton(v2d_v(0.4, 0.5), v2d_v(0.4, 0.1), fontSize, "load game", TEXT_JUSTIFICATION_RIGHT, PROGRAM_MODE_LOAD_GAME, color2, bgColor2);
//  mMainMenu->addButton(v2d_v(0.3, 0.5), v2d_v(0.5, 0.1), fontSize, "rogue viz", TEXT_JUSTIFICATION_RIGHT, PROGRAM_MODE_ROGUE, color2, bgColor2);
  mMainMenu->addButton(v2d_v(0.4, 0.65), v2d_v(0.4, 0.1), fontSize, "fullscreen", TEXT_JUSTIFICATION_RIGHT, PROGRAM_MODE_TOGGLE_FULLSCREEN, color2, bgColor2);
  mMainMenu->addButton(v2d_v(0.6, 0.8), v2d_v(0.2, 0.1), fontSize, "quit", TEXT_JUSTIFICATION_RIGHT, PROGRAM_QUIT, color2, bgColor2);
}

// main program loop * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int Longshot::loop() {
  printf("block_t size: %lu\n", sizeof (block_t));
  printf("PhysicsEntity size: %lu\n", sizeof (PhysicsEntity));

  // bail immediately if constructor failed at some point
  if (mConstructorSuccessful == false) {
    printf("Longshot::loop(): constructor failed!\n");
    return -1;
  }

  while (mProgramMode != PROGRAM_QUIT && mProgramMode != MENU_APPLICATION_QUIT) {
    // glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mMainMenu->draw();
    mGameWindow->swapBuffers();

    // figure out what the user wants to do
    mProgramMode = mMainMenu->gameMenuChoice(true);

    // and do it
    switch (mProgramMode) {
      case PROGRAM_MODE_NEW_GAME:
        printf("menu choice: new game\n");
        mGame = new game_c(mGameWindow);
        mGame->run(true);
        break;

      case PROGRAM_MODE_LOAD_GAME:
        printf("menu choice: load game\n");
        mGame = new game_c(mGameWindow);
        mGame->run(false);
        break;

      case PROGRAM_MODE_TOGGLE_FULLSCREEN:
        printf("menu choice: toggle fullscreen\n");
        mGameWindow->toggleFullscreen();

        // since the context has changed, ogl assets must be reloaded
        reloadMenu();
        break;

      // case PROGRAM_MODE_ROGUE:
      //   printf("menu choice: rogue_viz\n");
      //   mRogueViewer = new RogueViewer(mGameWindow, );
      //   mRogueViewer->start();
      //   delete mRogueViewer;
      //   break;

      case MENU_APPLICATION_QUIT:
      case PROGRAM_QUIT:
        printf("menu choice: quit\n");
        break;

      default:
        break;
    }

    if (mGame != NULL) {
      delete mGame;
      mGame = NULL;
    }
  }

  return 0;
}
