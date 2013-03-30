// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Texture
// *
// * desc:
// *
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef Texture_h_
#define Texture_h_

#include <string>

#include "SdlInterface.h"

class Texture {
public:
	Texture(std::string fileName);
	~Texture();

	int loadTexture();

	void bind();

	std::string mFileName;
	GLuint mWidth;
	GLuint mHeight;
	GLuint mGlHandle;
};


#endif // Texture_h_
