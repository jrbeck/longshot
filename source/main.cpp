// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * main.cpp
// *
// * Longshot driver
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "game/Longshot.h"

// program entry point
int main(int argc, char* argv[]) {
  Longshot* longshot;
  longshot = new Longshot;
  int value = longshot->loop();
  delete longshot;
  return value;
}
