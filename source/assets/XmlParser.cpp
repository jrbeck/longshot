#include "XmlParser.h"

XmlParser::XmlParser () {
  mInputFile = NULL;
}

XmlParser::~XmlParser () {
  closeFile ();
}

int XmlParser::openFile (const char *fileName) {
  closeFile ();

  mInputFile = fopen (fileName, "rt");

  if (mInputFile == NULL) {
    return 1;
  }

  return 0;
}

int XmlParser::closeFile (void) {
  if (mInputFile != NULL) {
    fclose (mInputFile);
  }

  return 0;
}

xml_element_t XmlParser::getNextElement (void) {
  if (mInputFile == NULL) {
    xml_element_t ret;
    ret.type = XML_TYPE_INVALID;
  }

  int c = getc(mInputFile);
  while (c != EOF) {
    // ignore newlines, carriage returns, and whitespace
    if (c == '\n' || c == '\r' || c == ' ') {
      c = getc(mInputFile);
    }
    else {
      // put the char back into the file buffer
      ungetc(c, mInputFile);

      if (c == '<') {
        return readTag ();
      }
      else {
        return readDatum ();
      }
    }
  }

  xml_element_t ret;
  ret.type = XML_TYPE_INVALID;

  return ret;
}

// returns a tag string or signals a closure
xml_element_t XmlParser::readTag (void) {
  char c = 0;
  xml_element_t ret;

  // make sure we're at the beginning
  while (c != '<') {
    c = getc(mInputFile);
    if (c == EOF) {
      ret.type = XML_TYPE_INVALID;
      return ret;
    }
  }

  // check for a closure tag
  if ((c = getc(mInputFile)) == '/') {
    ret.type = XML_TYPE_CLOSE;
  }
  else {
    ret.type = XML_TYPE_OPEN;
    ret.text += c;
  }

  // read the rest of the tag
  while (1) {
    c = getc(mInputFile);
    if (c != '>' && c != EOF) {
      ret.text += c;
    }
    else break;
  }

  if (c == EOF) {
    ret.type = XML_TYPE_INVALID;
  }

  return ret;
}

// returns a single datum as a string, or null if none exists
xml_element_t XmlParser::readDatum (void) {
  char c;
  xml_element_t ret;

  do {
    c = getc(mInputFile);

    if (c != '<' && c != '\n' && c != '\r') ret.text += c;
  } while (c != '<' && c != EOF);

  if (c == EOF) ret.type = XML_TYPE_INVALID;
  else {
    ret.type = XML_TYPE_DATUM;

    // put that '<' back into the buffer
    ungetc(c, mInputFile);
  }

  return ret;
}
