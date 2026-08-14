#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#if defined(_WIN32)
__declspec(dllimport) int __stdcall SetEvent(void *);
#endif

int main(int argc, char *argv[]) {
#if defined(_WIN32)
  void *handle = 0;

  if ((2 != argc) || (1 != sscanf(argv[1], "%p", &handle))) {
    return 2;
  }

  SetEvent(handle);
  return 0;
#else
  (void)argc;
  (void)argv;
  return 0;
#endif
}
