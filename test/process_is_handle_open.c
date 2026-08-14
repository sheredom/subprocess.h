#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#if defined(_WIN32)
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
__declspec(dllimport) int __stdcall GetHandleInformation(void *,
                                                          unsigned long *);
#endif

int main(int argc, char *argv[]) {
#if defined(_WIN32)
  const unsigned long error_invalid_handle = 6;
  unsigned long flags = 0;
  void *handle = 0;

  if ((2 != argc) || (1 != sscanf(argv[1], "%p", &handle))) {
    return 2;
  }

  if (GetHandleInformation(handle, &flags)) {
    return 1;
  }

  return error_invalid_handle == GetLastError() ? 0 : 2;
#else
  (void)argc;
  (void)argv;
  return 0;
#endif
}
