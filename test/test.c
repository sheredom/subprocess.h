#include "subprocess.h"
#include "utest.h"

#include <stdlib.h>

#define SUBPROCESS_SUITE c
#include "test_shared.h"

UTEST(c, create_does_not_inherit_another_subprocess_pipe) {
#if defined(_WIN32)
  UTEST_SKIP("POSIX file-descriptor test");
#else
  const char *const first_command_line[] = {"./process_return_stdin_count", 0};
  const char *second_command_line[] = {"./process_is_fd_open", 0, 0};
  struct subprocess_s first_process;
  struct subprocess_s second_process;
  char fd_argument[32];
  int inherited_fd;
  int second_return_code = -1;

  ASSERT_EQ(0, subprocess_create(first_command_line, 0, &first_process));

  /* A later child must not inherit the parent's write end of the first
     child's stdin pipe, or it can prevent the first child from seeing EOF. */
  inherited_fd = fileno(subprocess_stdin(&first_process));
  ASSERT_TRUE(0 <= inherited_fd);
  ASSERT_TRUE(
      0 < snprintf(fd_argument, sizeof(fd_argument), "%d", inherited_fd));
  second_command_line[1] = fd_argument;

  ASSERT_EQ(0, subprocess_create(second_command_line, 0, &second_process));
  ASSERT_EQ(0, subprocess_join(&second_process, &second_return_code));
  ASSERT_EQ(0, subprocess_destroy(&second_process));

  ASSERT_EQ(0, subprocess_terminate(&first_process));
  ASSERT_EQ(0, subprocess_join(&first_process, 0));
  ASSERT_EQ(0, subprocess_destroy(&first_process));

  EXPECT_EQ_MSG(0, second_return_code,
                "the second subprocess inherited the first subprocess pipe");
#endif
}

UTEST(c, create_gives_the_child_a_stdin_when_fd_zero_was_free) {
#if defined(_WIN32)
  UTEST_SKIP("POSIX file-descriptor test");
#else
  const char *const command_line[] = {"./process_is_fd_open", "0", 0};
  struct subprocess_s process;
  int saved_stdin;
  int restored;
  int created;
  int joined = -1;
  int return_code = -1;

  /* With fd 0 free the stdin pipe's read end lands on it, so the child's
     dup2 onto STDIN_FILENO becomes dup2(0, 0) -- a no-op that clears no
     FD_CLOEXEC. */
  saved_stdin = dup(STDIN_FILENO);
  ASSERT_TRUE(0 <= saved_stdin);
  ASSERT_EQ(0, close(STDIN_FILENO));

  /* No assertions until stdin is back: one would return early and leave every
     later test in this binary without a stdin. */
  created = subprocess_create(command_line, 0, &process);
  if (0 == created) {
    joined = subprocess_join(&process, &return_code);
    subprocess_destroy(&process);
  }

  restored = dup2(saved_stdin, STDIN_FILENO);
  close(saved_stdin);

  ASSERT_TRUE(0 <= restored);
  ASSERT_EQ(0, created);
  ASSERT_EQ(0, joined);
  EXPECT_EQ_MSG(1, return_code, "exec closed the child's stdin");
#endif
}

#if defined(_WIN32)
UTEST(c, create_does_not_inherit_unlisted_windows_handle) {
  const unsigned long wait_timeout = 0x00000102;
  const char *command_line[] = {"./process_signal_handle", 0, 0};
  struct subprocess_security_attributes_s security_attributes = {
      sizeof(security_attributes), SUBPROCESS_NULL, 1};
  struct subprocess_s process;
  char handle_argument[32];
  void *inheritable_handle;
  unsigned long wait_result;
  int return_code = -1;

  inheritable_handle = CreateEventA(
      SUBPROCESS_PTR_CAST(LPSECURITY_ATTRIBUTES, &security_attributes), 1, 0,
      SUBPROCESS_NULL);
  ASSERT_TRUE(inheritable_handle);
  ASSERT_TRUE(0 < snprintf(handle_argument, sizeof(handle_argument), "%p",
                           inheritable_handle));
  command_line[1] = handle_argument;

  ASSERT_EQ(0, subprocess_create(command_line, 0, &process));
  ASSERT_EQ(0, subprocess_join(&process, &return_code));
  ASSERT_EQ(0, subprocess_destroy(&process));
  wait_result = WaitForSingleObject(inheritable_handle, 0);
  ASSERT_TRUE(CloseHandle(inheritable_handle));

  EXPECT_EQ(0, return_code);
  EXPECT_EQ_MSG(wait_timeout, wait_result,
                "subprocess inherited a handle outside its standard streams");
}
#endif

UTEST(c, create_keeps_pipe_ends_off_the_standard_descriptors) {
#if defined(_WIN32)
  UTEST_SKIP("POSIX file-descriptor test");
#else
  const char *const command_line[] = {"./process_return_zero", 0};
  struct subprocess_s process;
  int saved[3];
  int index;
  int created;
  int stdin_fd = -1;
  int stdout_fd = -1;
  int restored = 1;

  /* With 0, 1 and 2 all free the pipes land on them, and a later dup2 onto the
     same descriptor is a no-op that leaves FD_CLOEXEC set. */
  for (index = 0; index < 3; index++) {
    saved[index] = dup(index);
    ASSERT_TRUE(0 <= saved[index]);
  }
  for (index = 0; index < 3; index++) {
    close(index);
  }

  /* Nothing may assert or print until the standard descriptors are back. */
  created = subprocess_create(command_line, 0, &process);
  if (0 == created) {
    if (subprocess_stdin(&process)) {
      stdin_fd = fileno(subprocess_stdin(&process));
    }
    if (subprocess_stdout(&process)) {
      stdout_fd = fileno(subprocess_stdout(&process));
    }
    subprocess_join(&process, 0);
    subprocess_destroy(&process);
  }

  for (index = 0; index < 3; index++) {
    if (-1 == dup2(saved[index], index)) {
      restored = 0;
    }
    close(saved[index]);
  }

  ASSERT_TRUE(restored);
  ASSERT_EQ(0, created);
  EXPECT_GT_MSG(stdin_fd, STDERR_FILENO,
                "a pipe end sits on a standard descriptor");
  EXPECT_GT_MSG(stdout_fd, STDERR_FILENO,
                "a pipe end sits on a standard descriptor");
#endif
}

#if defined(_WIN32)
UTEST_C_FUNC __declspec(dllimport) void *__stdcall
OpenProcess(unsigned long, int, unsigned long);
#endif

static int subprocess_test_pid_alive(unsigned long pid) {
#if defined(_WIN32)
  const unsigned long synchronize = 0x00100000;
  const unsigned long wait_object_0 = 0x00000000;
  void *process = OpenProcess(synchronize, 0, pid);
  int alive;

  if (!process) {
    return 0;
  }
  alive = wait_object_0 != WaitForSingleObject(process, 0);
  CloseHandle(process);
  return alive;
#else
  if ((0 != kill((pid_t)pid, 0)) && (EPERM != errno)) {
    return 0;
  }
#if defined(__linux__)
  {
    char path[64];
    char status[512];
    char *command_end;
    FILE *status_file;

    if (0 < snprintf(path, sizeof(path), "/proc/%lu/stat", pid)) {
      status_file = fopen(path, "r");
      if (status_file) {
        if (fgets(status, sizeof(status), status_file)) {
          command_end = strrchr(status, ')');
          if (command_end && (' ' == command_end[1]) &&
              ('Z' == command_end[2])) {
            fclose(status_file);
            return 0;
          }
        }
        fclose(status_file);
      }
    }
  }
#endif
  return 1;
#endif
}

static int subprocess_test_parse_pids(const char *text, unsigned long *child,
                                      unsigned long *descendant) {
  char *end;

  errno = 0;
  *child = strtoul(text, &end, 10);
  if ((0 != errno) || (end == text)) {
    return 0;
  }

  text = end;
  *descendant = strtoul(text, &end, 10);
  return (0 == errno) && (end != text);
}

UTEST(c, create_terminate_on_parent_exit_failure_does_not_leak) {
  const char *const command_line[] = {
      "./subprocess_this_command_should_not_exist", 0};
  struct subprocess_s process;
  int before;
  int after;
  int attempt;

  before = subprocess_test_open_resource_count();
  ASSERT_TRUE(0 <= before);

  for (attempt = 0; attempt < 5; attempt++) {
    ASSERT_EQ(subprocess_error_not_found,
              subprocess_create(command_line,
                                subprocess_option_terminate_on_parent_exit,
                                &process));
  }

  after = subprocess_test_open_resource_count();
  ASSERT_TRUE(0 <= after);
  ASSERT_EQ(before, after);
}

UTEST(c, create_terminate_on_parent_exit) {
  const char *const command_line[] = {"./process_parent_death_parent", 0};
  struct subprocess_s process;
  char child_pids[128];
  unsigned long child_pid = 0;
  unsigned long descendant_pid = 0;
  int return_code = -1;
  int attempt;

  ASSERT_EQ(0, subprocess_create(command_line, 0, &process));
  ASSERT_TRUE(
      fgets(child_pids, sizeof(child_pids), subprocess_stdout(&process)) != 0);
  ASSERT_TRUE(
      subprocess_test_parse_pids(child_pids, &child_pid, &descendant_pid));
  ASSERT_EQ(0, subprocess_terminate(&process));
  ASSERT_EQ(0, subprocess_join(&process, &return_code));
  ASSERT_NE(0, return_code);
  ASSERT_EQ(0, subprocess_destroy(&process));

  for (attempt = 0; attempt < 50; attempt++) {
    if (!subprocess_test_pid_alive(child_pid) &&
        !subprocess_test_pid_alive(descendant_pid)) {
      break;
    }
#if defined(_WIN32)
    Sleep(100);
#else
    usleep(100000);
#endif
  }

  ASSERT_FALSE(subprocess_test_pid_alive(child_pid));
  ASSERT_FALSE(subprocess_test_pid_alive(descendant_pid));
}

UTEST(c, destroy_terminate_on_parent_exit) {
  const char *const command_line[] = {"./process_parent_death_child", 0};
  struct subprocess_s process;
  char child_pids[128];
  unsigned long child_pid = 0;
  unsigned long descendant_pid = 0;
  int attempt;

  ASSERT_EQ(0, subprocess_create(command_line,
                                 subprocess_option_terminate_on_parent_exit,
                                 &process));
  ASSERT_TRUE(
      fgets(child_pids, sizeof(child_pids), subprocess_stdout(&process)) != 0);
  ASSERT_TRUE(
      subprocess_test_parse_pids(child_pids, &child_pid, &descendant_pid));
  ASSERT_EQ(0, subprocess_destroy(&process));

  for (attempt = 0; attempt < 50; attempt++) {
    if (!subprocess_test_pid_alive(child_pid) &&
        !subprocess_test_pid_alive(descendant_pid)) {
      break;
    }
#if defined(_WIN32)
    Sleep(100);
#else
    usleep(100000);
#endif
  }

  ASSERT_FALSE(subprocess_test_pid_alive(child_pid));
  ASSERT_FALSE(subprocess_test_pid_alive(descendant_pid));
}
