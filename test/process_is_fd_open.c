#if !defined(_WIN32)
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#endif

int main(int argc, char *argv[]) {
#if defined(_WIN32)
  (void)argc;
  (void)argv;
  return 0;
#else
  int fd;

  if (2 != argc) {
    return 2;
  }

  fd = atoi(argv[1]);
  errno = 0;
  if (-1 == fcntl(fd, F_GETFD)) {
    return EBADF == errno ? 0 : 2;
  }

  return 1;
#endif
}
