#include "subprocess.h"
#include "utest.h"

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
