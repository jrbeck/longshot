#pragma once

#ifdef _WIN32
  #include <Windows.h>
#else
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <dirent.h>
  #include <stdio.h>
  #include <string.h>
  typedef unsigned char BYTE;
  typedef unsigned int UINT;
#endif

#include <cstdio>


class FileSystem {
public:
  // create the save directory if it doesn't exist
  static int createFolder(const char* path);
  static int deleteAllFilesInFolder(const char* path);
};
