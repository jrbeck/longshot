#include "FileSystem.h"

#ifdef _WIN32

int FileSystem::createFolder(const char* path) {
  // CreateDirectory(TEXT(path), NULL);
  CreateDirectory(TEXT("save"), NULL);
  return 0;
}

int FileSystem::deleteAllFilesInFolder(const char* path) {
  // LPWSTR folderPath = TEXT(path);
  LPWSTR folderPath = TEXT("save");
  WIN32_FIND_DATA info;
  HANDLE hp;

  wchar_t findPath[260];
  wsprintf(findPath, TEXT("%s\\*.*"), folderPath);
  hp = FindFirstFile(findPath, &info);
  wchar_t fullName[260];
  int numFilesDeleted = 0;

  do {
    wsprintf(fullName, TEXT("%s\\%s"), folderPath, info.cFileName);

    if (!(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      numFilesDeleted++;
//      wprintf (TEXT ("deleting: %s\n"), fullName);
      DeleteFile(fullName);
    }
  } while (FindNextFile(hp, &info));

  printf("deleted: %d files\n", numFilesDeleted);

  FindClose(hp);

  return 0;
}

#else // NON WINDOWS * * * * * * * * * * * * * * * *

int FileSystem::createFolder(const char* path) {
  // CreateDirectory(TEXT(path), NULL);
  return 0;
}

int FileSystem::deleteAllFilesInFolder(const char* path) {
//   LPWSTR folderPath = TEXT(path);
//   WIN32_FIND_DATA info;
//   HANDLE hp;

//   wchar_t findPath[260];
//   wsprintf(findPath, TEXT("%s\\*.*"), folderPath);
//   hp = FindFirstFile(findPath, &info);
//   wchar_t fullName[260];
//   int numFilesDeleted = 0;

//   do {
//     wsprintf(fullName, TEXT("%s\\%s"), folderPath, info.cFileName);

//     if (!(info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
//       numFilesDeleted++;
// //      wprintf (TEXT ("deleting: %s\n"), fullName);
//       DeleteFile(fullName);
//     }
//   } while (FindNextFile(hp, &info));

//   printf("deleted: %d files\n", numFilesDeleted);

//   FindClose(hp);
  return 0;
}

#endif

