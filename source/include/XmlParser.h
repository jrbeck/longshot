// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * XmlParser
// *
// * handles extremely basic XML stuff
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef XmlParser_h_
#define XmlParser_h_

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <string>

using namespace std;


typedef struct {
  int type;
  string text;
} xml_element_t;


enum {
  XML_TYPE_INVALID,
  XML_TYPE_OPEN,
  XML_TYPE_CLOSE,
  XML_TYPE_DATUM
};



class XmlParser
{
public:
  XmlParser ();
  ~XmlParser ();

  int openFile (const char *fileName);
  int closeFile (void);

  xml_element_t getNextElement (void);

private:
  xml_element_t readTag (void);
  xml_element_t readDatum (void);

  FILE *mInputFile;
};


#endif // XmlParser_h_
