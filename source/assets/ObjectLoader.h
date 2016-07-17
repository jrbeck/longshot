// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * ObjectLoader
// *
// * this loads simple XML object files into display lists
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#ifdef _WIN32
  #include <Windows.h>
#else
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

#include "GL/glut.h"

#include "v2d.h"
#include "v3d.h"
#include "xmlparser.h"


enum {
  TAG_DEPTH,
  TAG_V2D,
  TAG_COLOR,
  NUM_TAGS,
  TAG_UNDEFINED
};


class ObjectLoader {
public:
  ObjectLoader();
  ~ObjectLoader();

  static GLuint loadObjectToDisplayList(const char *fileName);

private:
  static int loadQuad(XmlParser& xmlParser);
};
