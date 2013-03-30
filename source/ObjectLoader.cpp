#include "ObjectLoader.h"


ObjectLoader::ObjectLoader () {
}



ObjectLoader::~ObjectLoader () {
}



GLuint ObjectLoader::loadObjectToDisplayList (const char *fileName) {
	GLuint displayListHandle;

	displayListHandle = glGenLists (1);
	glNewList (displayListHandle, GL_COMPILE);

	XmlParser xmlParser;

	xmlParser.openFile (fileName);

	xml_element_t e = xmlParser.getNextElement ();

//	printf ("\n");

	while (e.type != XML_TYPE_INVALID) {
		if (e.type == XML_TYPE_OPEN) {
			if (e.text.compare ("quad") == 0) {
				loadQuad (xmlParser);
			}
			else {
//				printf ("open: %s\n", e.text.c_str ());
			}
		}
		else if (e.type == XML_TYPE_DATUM) {
			v3d_t tri;
			sscanf (e.text.c_str (), "%lf %lf", &tri.x, &tri.y);
//			printf ("%s: (%f, %f)\n", e.text.c_str (), tri.x, tri.y);
		}
		else if (e.type == XML_TYPE_CLOSE) {
//			printf ("close: %s\n", e.text.c_str ());
		}

		e = xmlParser.getNextElement ();
	}
//	printf ("\nloaded object (%d)\n", displayListHandle);

	// all done with the display list
	glEndList();
	return displayListHandle;
}




int ObjectLoader::loadQuad (XmlParser &xmlParser) {
//	printf ("loading quad:\n");

	xml_element_t e = xmlParser.getNextElement ();

	double depth = 1.0;
	v2d_t vertices[4] = {0.0, 0.0, 0.0, 0.0};
	GLfloat colors[4] = {1.0, 1.0, 1.0, 1.0};

	int tagType = TAG_UNDEFINED;
	bool quit = false;

	bool colorSet = false;
	int currentVertex = 0;

	// read in the depth and vertices
	while (e.type != XML_TYPE_INVALID && !quit) {
		if (e.type == XML_TYPE_OPEN) {
			if (e.text.compare ("depth") == 0) {
				tagType = TAG_DEPTH;
			}
			else if (e.text.compare ("v2d") == 0) {
				tagType = TAG_V2D;
			}
			else if (e.text.compare ("color") == 0) {
				tagType = TAG_COLOR;
			}
			else { // invalid tag
//				printf ("ObjectLoader::loadQuad(): error: invalid tag\n");
				return 1;
			}
		}
		else if (e.type == XML_TYPE_DATUM) {

			if (tagType == TAG_DEPTH) {
				sscanf (e.text.c_str (), "%lf", &depth);
//				printf ("depth: (%f)\n", depth);
				
			}
			else if (tagType == TAG_V2D && currentVertex < 4) {
				sscanf (e.text.c_str (), "%lf %lf", &vertices[currentVertex].x, &vertices[currentVertex].y);
//				printf ("vertex: (%f, %f)\n", vertices[currentVertex].x, vertices[currentVertex].y);

				currentVertex++;
			}
			else if (tagType == TAG_COLOR) {
				sscanf (e.text.c_str (), "%f %f %f %f", &colors[0], &colors[1], &colors[2], &colors[3]);
//				printf ("color: (%f, %f, %f, %f)\n", colors[0], colors[1], colors[2], colors[3]);
				colorSet = true;
			}
		}
		else if (e.type == XML_TYPE_CLOSE) {
			if (e.text.compare ("quad") == 0) {
				quit = true;
			}
		}

		if (!quit) {
			e = xmlParser.getNextElement ();
		}
	}

	double zValue = depth * 0.5;

	if (colorSet) {
		glColor4fv (colors);
	}

	// draw the front
	glNormal3d (0.0, 0.0, -1.0);

//	glTexCoord2f (0.0f, 0.0f);
	glVertex3d (vertices[0].x, vertices[0].y, zValue);
//	glTexCoord2f (0.0f, 0.5f);
	glVertex3d (vertices[3].x, vertices[3].y, zValue);
//	glTexCoord2f (0.5f, 0.5f);
	glVertex3d (vertices[2].x, vertices[2].y, zValue);
//	glTexCoord2f (0.5f, 0.0f);
	glVertex3d (vertices[1].x, vertices[1].y, zValue);


	// back
	glNormal3d (0.0, 0.0, 1.0);

//	glTexCoord2f (0.0f, 0.0f);
	glVertex3d (vertices[0].x, vertices[0].y, -zValue);
//	glTexCoord2f (0.0f, 0.5f);
	glVertex3d (vertices[1].x, vertices[1].y, -zValue);
//	glTexCoord2f (0.5f, 0.5f);
	glVertex3d (vertices[2].x, vertices[2].y, -zValue);
//	glTexCoord2f (0.5f, 0.0f);
	glVertex3d (vertices[3].x, vertices[3].y, -zValue);



	// top
	glNormal3d (0.0, 1.0, 0.0);

//	glTexCoord2f (0.0f, 0.0f);
	glVertex3d (vertices[1].x, vertices[1].y, zValue);
//	glTexCoord2f (0.0f, 0.5f);
	glVertex3d (vertices[2].x, vertices[2].y, zValue);
//	glTexCoord2f (0.5f, 0.5f);
	glVertex3d (vertices[2].x, vertices[2].y, -zValue);
//	glTexCoord2f (0.5f, 0.0f);
	glVertex3d (vertices[1].x, vertices[1].y, -zValue);



	// bottom
	glNormal3d (0.0, -1.0, 0.0);

//	glTexCoord2f (0.0f, 0.0f);
	glVertex3d (vertices[0].x, vertices[0].y, zValue);
//	glTexCoord2f (0.0f, 0.5f);
	glVertex3d (vertices[0].x, vertices[0].y, -zValue);
//	glTexCoord2f (0.5f, 0.5f);
	glVertex3d (vertices[3].x, vertices[3].y, -zValue);
//	glTexCoord2f (0.5f, 0.0f);
	glVertex3d (vertices[3].x, vertices[3].y, zValue);



	// left
	glNormal3d (1.0, 0.0, 0.0);

//	glTexCoord2f (0.0f, 0.0f);
	glVertex3d (vertices[1].x, vertices[1].y, zValue);
//	glTexCoord2f (0.0f, 0.5f);
	glVertex3d (vertices[1].x, vertices[1].y, -zValue);
//	glTexCoord2f (0.5f, 0.5f);
	glVertex3d (vertices[0].x, vertices[0].y, -zValue);
//	glTexCoord2f (0.5f, 0.0f);
	glVertex3d (vertices[0].x, vertices[0].y, zValue);



	// right
	glNormal3d (-1.0, 0.0, 0.0);

//	glTexCoord2f (0.0f, 0.0f);
	glVertex3d (vertices[2].x, vertices[2].y, zValue);
//	glTexCoord2f (0.0f, 0.5f);
	glVertex3d (vertices[3].x, vertices[3].y, zValue);
//	glTexCoord2f (0.5f, 0.5f);
	glVertex3d (vertices[3].x, vertices[3].y, -zValue);
//	glTexCoord2f (0.5f, 0.0f);
	glVertex3d (vertices[2].x, vertices[2].y, -zValue);



	return 0;
}

