// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

#include <string.h>

#if defined(_WIN32)
#include <direct.h>
#define subprocess_test_getcwd _getcwd
#else
#include <unistd.h>
#define subprocess_test_getcwd getcwd
#endif

int main(int argc, char *argv[]) {
  char cwd[4096];

  if (argc < 2) {
    return 1;
  }

  if (!subprocess_test_getcwd(cwd, sizeof(cwd))) {
    return 2;
  }

  return 0 == strcmp(cwd, argv[1]) ? 0 : 3;
}
