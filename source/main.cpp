// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * main.cpp
// *
// * Longshot driver
// *
// * by john beck
// * last modified: 2013-03-30
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#include "Longshot.h"

int main(int argc, char *argv[]);

// program entry point
int main(int argc, char *argv[]) {
	Longshot *longshot;
	longshot = new Longshot;
	int value = longshot->loop();
	delete longshot;
	return value;
}
