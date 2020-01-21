// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>

#include "utest.h"

#include "subprocess.h"

UTEST(create, subprocess_return_zero) {
  const char *const commandLine[] = {"./process_return_zero", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_return_fortytwo) {
  const char *const commandLine[] = {"./process_return_fortytwo", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(42, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_return_argc) {
  const char *const commandLine[] = {
      "./process_return_argc", "foo", "bar", "baz", "faz", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(5, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_return_argv) {
  const char *const commandLine[] = {"./process_return_argv", "13", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(13, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_return_stdin) {
  const char *const commandLine[] = {"./process_return_stdin", 0};
  struct subprocess_s process;
  int ret = -1;
  FILE *stdin_file;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  stdin_file = subprocess_stdin(&process);
  ASSERT_TRUE(stdin_file);

  ASSERT_EQ('a', putc('a', stdin_file));
  ASSERT_EQ('b', putc('b', stdin_file));
  ASSERT_EQ('b', putc('b', stdin_file));
  ASSERT_EQ('a', putc('a', stdin_file));
  ASSERT_EQ(' ', putc(' ', stdin_file));
  ASSERT_EQ('a', putc('a', stdin_file));
  ASSERT_EQ('r', putc('r', stdin_file));
  ASSERT_EQ('e', putc('e', stdin_file));
  ASSERT_EQ(' ', putc(' ', stdin_file));
  ASSERT_EQ('g', putc('g', stdin_file));
  ASSERT_EQ('r', putc('r', stdin_file));
  ASSERT_EQ('e', putc('e', stdin_file));
  ASSERT_EQ('a', putc('a', stdin_file));
  ASSERT_EQ('t', putc('t', stdin_file));
  ASSERT_EQ('!', putc('!', stdin_file));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_return_stdin_count) {
  const char *const commandLine[] = {"./process_return_stdin_count", 0};
  struct subprocess_s process;
  int ret = -1;
  FILE *stdin_file;
  const char temp[41] = "Wee, sleekit, cow'rin, tim'rous beastie!";

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  stdin_file = subprocess_stdin(&process);
  ASSERT_TRUE(stdin_file);

  ASSERT_NE(EOF, fputs(temp, stdin_file));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(40, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_stdout_argc) {
  const char *const commandLine[] = {
      "./process_stdout_argc", "foo", "bar", "baz", "faz", 0};
  struct subprocess_s process;
  int ret = -1;
  FILE *stdout_file;
  char temp[32];

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  stdout_file = subprocess_stdout(&process);
  ASSERT_TRUE(stdout_file);

  ASSERT_TRUE(fgets(temp, 32, stdout_file));

  ASSERT_EQ(5, atoi(temp));

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_stdout_argv) {
  const char *const commandLine[] = {
      "./process_stdout_argv", "foo", "bar", "baz", "faz", 0};
  struct subprocess_s process;
  int ret = -1;
  FILE *stdout_file;
  char temp[16];
  const char compare[16] = "foo bar baz faz";

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  stdout_file = subprocess_stdout(&process);
  ASSERT_TRUE(stdout_file);

  ASSERT_TRUE(fgets(temp, 16, stdout_file));

  ASSERT_STREQ(temp, compare);

  ASSERT_TRUE(fgets(temp, 16, stdout_file)); // should contain trailing newline character(s)
  ASSERT_FALSE(fgets(temp, 16, stdout_file)); // should be at EOF now
  ASSERT_TRUE(feof(stdout_file));

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_stderr_argc) {
  const char *const commandLine[] = {
      "./process_stderr_argc", "foo", "bar", "baz", "faz", 0};
  struct subprocess_s process;
  int ret = -1;
  FILE *stderr_file;
  char temp[32];

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  stderr_file = subprocess_stderr(&process);
  ASSERT_TRUE(stderr_file);

  ASSERT_TRUE(fgets(temp, 32, stderr_file));

  ASSERT_EQ(5, atoi(temp));

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_stderr_argv) {
  const char *const commandLine[] = {
      "./process_stderr_argv", "foo", "bar", "baz", "faz", 0};
  struct subprocess_s process;
  int ret = -1;
  FILE *stderr_file;
  char temp[16];
  const char compare[16] = "foo bar baz faz";

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  stderr_file = subprocess_stderr(&process);
  ASSERT_TRUE(stderr_file);

  ASSERT_TRUE(fgets(temp, 16, stderr_file));

  ASSERT_STREQ(temp, compare);

  ASSERT_TRUE(fgets(temp, 16, stderr_file)); // should contain trailing newline character(s)
  ASSERT_FALSE(fgets(temp, 16, stderr_file)); // should be at EOF now
  ASSERT_TRUE(feof(stderr_file));

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_combined_stdout_stderr) {
  const char *const commandLine[] = {"./process_combined_stdout_stderr", 0};
  struct subprocess_s process;
  int ret = -1;
  FILE *stdout_file;
  FILE *stderr_file;
  char temp[25];
  const char compare[25] = "Hello,It's me!world!Yay!";

  ASSERT_EQ(0, subprocess_create(commandLine,
                                 subprocess_option_combined_stdout_stderr,
                                 &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  stdout_file = subprocess_stdout(&process);
  ASSERT_TRUE(stdout_file);

  stderr_file = subprocess_stderr(&process);
  ASSERT_FALSE(stderr_file);

  ASSERT_TRUE(fgets(temp, 25, stdout_file));

  ASSERT_STREQ(compare, temp);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_not_inherit_environment) {
  const char *const commandLine[] = {"./process_inherit_environment", 0};
  struct subprocess_s process;
  int ret = -1;

#ifdef _MSC_VER
  ASSERT_TRUE(SetEnvironmentVariable("PROCESS_ENV_TEST", "1"));
#else
  ASSERT_FALSE(putenv("PROCESS_ENV_TEST=1"));
#endif

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_inherit_environment) {
  const char *const commandLine[] = {"./process_inherit_environment", 0};
  struct subprocess_s process;
  int ret = -1;

#ifdef _MSC_VER
  ASSERT_TRUE(SetEnvironmentVariable("PROCESS_ENV_TEST", "42"));
#else
  ASSERT_FALSE(putenv("PROCESS_ENV_TEST=42"));
#endif

  ASSERT_EQ(0,
            subprocess_create(commandLine,
                              subprocess_option_inherit_environment, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(42, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_not_inherit_all_environment) {
  const char *const commandLine[] = {"./process_inherit_environment", "all", 0};
  struct subprocess_s process;
  int ret = -1;

#ifdef _MSC_VER
  ASSERT_TRUE(SetEnvironmentVariable("PROCESS_ENV_TEST", "42"));
#else
  ASSERT_FALSE(putenv("PROCESS_ENV_TEST=42"));
#endif

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_inherit_all_environment) {
  const char *const commandLine[] = {"./process_inherit_environment", "all", 0};
  struct subprocess_s process;
  int ret = -1;

#ifdef _MSC_VER
  ASSERT_TRUE(SetEnvironmentVariable("PROCESS_ENV_TEST", "42"));
#else
  ASSERT_FALSE(putenv("PROCESS_ENV_TEST=42"));
#endif

  ASSERT_EQ(0,
            subprocess_create(commandLine,
                              subprocess_option_inherit_environment, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(1, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_fail_divzero) {
  const char *const commandLine[] = {"./process_fail_divzero", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0,subprocess_create(commandLine, 0, &process));
  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, EXIT_FAILURE);
}

UTEST(create, subprocess_fail_stackoverflow) {
  const char *const commandLine[] = {"./process_fail_stackoverflow", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0,subprocess_create(commandLine, 0, &process));
  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, EXIT_FAILURE);
}


UTEST_MAIN()
