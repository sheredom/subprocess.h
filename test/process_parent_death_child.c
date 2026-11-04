#include <stdio.h>

#include "subprocess.h"

#if defined(_WIN32)
__declspec(dllimport) unsigned long __stdcall GetProcessId(void *);
#else
#include <unistd.h>
#endif

int main(void) {
  const char *const command_line[] = {"./process_hung", 0};
  struct subprocess_s process;
  unsigned long child_pid;
  unsigned long descendant_pid;

  if (0 != subprocess_create(command_line, 0, &process)) {
    return 1;
  }

#if defined(_WIN32)
  child_pid = GetCurrentProcessId();
  descendant_pid = GetProcessId(process.hProcess);
#else
  child_pid = (unsigned long)getpid();
  descendant_pid = (unsigned long)process.child;
#endif

  printf("%lu %lu\n", child_pid, descendant_pid);
  fflush(stdout);

  return subprocess_join(&process, 0);
}
