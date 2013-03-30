// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * ObjectLoader
// *
// * this loads simple XML object files into display lists
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef ObjectLoader_h_
#define ObjectLoader_h_

#include <Windows.h>
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
	ObjectLoader ();
	~ObjectLoader ();

	static GLuint loadObjectToDisplayList (const char *fileName);

private:
	static int loadQuad (XmlParser &xmlParser);
};




#endif // ObjectLoader_h_