#include <stdio.h>

#include "subprocess.h"

int main(void) {
  const char *const command_line[] = {"./process_parent_death_child", 0};
  struct subprocess_s process;
  char child_pids[128];

  if (0 != subprocess_create(command_line,
                             subprocess_option_terminate_on_parent_exit,
                             &process)) {
    return 1;
  }

  if (!fgets(child_pids, sizeof(child_pids), subprocess_stdout(&process))) {
    return 1;
  }

  fputs(child_pids, stdout);
  fflush(stdout);

  // Wait forever. The test terminates this parent abruptly and verifies that
  // the managed subprocess tree is then terminated with it.
  return subprocess_join(&process, 0);
}
