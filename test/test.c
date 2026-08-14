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

UTEST(c, create_does_not_inherit_unlisted_windows_handle) {
#if defined(_WIN32)
  const char *command_line[] = {"./process_is_handle_open", 0, 0};
  struct subprocess_security_attributes_s security_attributes = {
      sizeof(security_attributes), SUBPROCESS_NULL, 1};
  struct subprocess_s process;
  char handle_argument[32];
  void *inheritable_handle;
  int return_code = -1;

  inheritable_handle =
      CreateEventA(&security_attributes, 1, 0, SUBPROCESS_NULL);
  ASSERT_TRUE(inheritable_handle);
  ASSERT_TRUE(0 < snprintf(handle_argument, sizeof(handle_argument), "%p",
                           inheritable_handle));
  command_line[1] = handle_argument;

  ASSERT_EQ(0, subprocess_create(command_line, 0, &process));
  ASSERT_EQ(0, subprocess_join(&process, &return_code));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_TRUE(CloseHandle(inheritable_handle));

  EXPECT_EQ_MSG(0, return_code,
                "subprocess inherited a handle outside its standard streams");
#else
  UTEST_SKIP("Windows handle-inheritance test");
#endif
}
