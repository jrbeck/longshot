// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * main.cpp
// *
// * Longshot driver
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "Longshot.h"

int main(int argc, char* argv[]);

// program entry point
int main(int argc, char* argv[]) {
  Longshot* longshot;
  longshot = new Longshot;
  int value = longshot->loop();
  delete longshot;
  return value;
}
