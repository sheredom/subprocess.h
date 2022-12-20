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

#if defined(_MSC_VER)
__declspec(dllimport) void __stdcall Sleep(unsigned long);
__declspec(dllimport) int __stdcall SetEnvironmentVariableA(const char *,
                                                            const char *);
#else
#include <unistd.h>
#endif

#include "subprocess.h"

UTEST(create, subprocess_destroy_is_idempotent) {
  const char *const commandLine[] = {"./process_return_zero", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, subprocess_destroy(&process));

  ASSERT_EQ(0, subprocess_destroy(&process));
}

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
  const char *const commandLine[] = {"./process_return_argv", "\n\r\n\r13", 0};
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

  ASSERT_TRUE(fgets(
      temp, 16, stdout_file)); // should contain trailing newline character(s)
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

  ASSERT_TRUE(fgets(
      temp, 16, stderr_file)); // should contain trailing newline character(s)
  ASSERT_FALSE(fgets(temp, 16, stderr_file)); // should be at EOF now
  ASSERT_TRUE(feof(stderr_file));

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_return_special_argv) {
  const char *const commandLine[] = {"./process_return_special_argv",
                                     "foo\nbar", "\"baz\"", "faz\\\"faz", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));
  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(7, ret); // 0b111

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_return_lpcmdline) {
  const char *const commandLine[] = {"./process_return_lpcmdline",
                                     "noquotes", "should be quoted", 0};
  struct subprocess_s process;
  int ret = -1;
  size_t cmp_index, index;
  FILE *stdout_file;
  char temp[32767];
  const char compare[28] = "noquotes \"should be quoted\"";

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  stdout_file = subprocess_stdout(&process);
  ASSERT_TRUE(stdout_file);

  ASSERT_TRUE(fgets(temp, 32767, stdout_file));

  // comparing from the back skips exe name
  cmp_index = strlen(compare) - 1;
  for (index = strlen(temp) - 1; index != 0 && cmp_index != 0; index--,cmp_index--) {
    if (temp[index] != compare[cmp_index])
      ASSERT_TRUE(0);
  }
  ASSERT_TRUE(1);

  ASSERT_FALSE(fgets(temp, 16, stdout_file)); // should be at EOF now
  ASSERT_TRUE(feof(stdout_file));

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
  ASSERT_TRUE(SetEnvironmentVariableA("PROCESS_ENV_TEST", "1"));
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
  ASSERT_TRUE(SetEnvironmentVariableA("PROCESS_ENV_TEST", "42"));
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
  ASSERT_TRUE(SetEnvironmentVariableA("PROCESS_ENV_TEST", "42"));
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
  ASSERT_TRUE(SetEnvironmentVariableA("PROCESS_ENV_TEST", "42"));
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

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));
  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));

  // On AArch64 systems divide by zero does not cause a failure.
#if !((defined(__arm64__) && defined(__APPLE__)) || defined(__aarch64__))
  ASSERT_NE(ret, 0);
#endif
}

UTEST(create, subprocess_fail_stackoverflow) {
  const char *const commandLine[] = {"./process_fail_stackoverflow", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));
  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_NE(ret, 0);
}

UTEST(create, subprocess_hung) {
  const char *const commandLine[] = {"./process_hung", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));
#if defined(_MSC_VER)
  Sleep(1000);
#else
  sleep(1);
#endif
  ASSERT_EQ(0, subprocess_terminate(&process));
  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_NE(ret, 0);
}

UTEST(create, subprocess_read) {
  const char *const commandLine[] = {"./process_stdout_data", "1048576", 0};
  struct subprocess_s process;
  int ret = -1;
  static char data[1048576 + 1] = {0};
  unsigned index = 0;
  FILE *out;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  out = subprocess_stdout(&process);

  ASSERT_EQ(1048576u, fread(data, 1, sizeof(data) - 1, out));

  for (index = 0; index < sizeof(data) - 1; index++) {
    const char table[17] = "0123456789abcdef";
    ASSERT_EQ(table[index % 16], data[index]);
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, 0);
}

UTEST(subprocess, stdout_fgetc) {
  const char *const commandLine[] = {"./process_stdout_large", "1024", 0};
  struct subprocess_s process;
  int ret = -1;
  static char data[1048576 + 1] = {0};
  char *cur = data;
  unsigned index = 0;
  FILE *out;
  int next;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  out = subprocess_stdout(&process);

  for (next = fgetc(out); EOF != next; next = fgetc(out)) {
    *cur++ = (char)next;
  }

  for (index = 0; index < 1024; index++) {
    const char *const helloWorld = "Hello, world!";
    ASSERT_TRUE(0 == memcmp(data + (index * strlen(helloWorld)), helloWorld,
                            strlen(helloWorld)));
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, 0);
}

UTEST(subprocess, stdout_fread_all) {
  const char *const commandLine[] = {"./process_stdout_large", "16384", 0};
  struct subprocess_s process;
  int ret = -1;
  static char data[1048576 + 1] = {0};
  unsigned index = 0;
  FILE *out;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  out = subprocess_stdout(&process);

  ASSERT_EQ(212992u, fread(data, 1, sizeof(data) - 1, out));
  ASSERT_NE(0, feof(out));

  for (index = 0; index < 16384; index++) {
    const char *const helloWorld = "Hello, world!";
    ASSERT_TRUE(0 == memcmp(data + (index * strlen(helloWorld)), helloWorld,
                            strlen(helloWorld)));
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, 0);
}

UTEST(subprocess, read_stdout) {
  const char *const commandLine[] = {"./process_stdout_large", "16384", 0};
  struct subprocess_s process;
  int ret = -1;
  static char data[1048576 + 1] = {0};
  unsigned index = 0;
  unsigned bytes_read = 0;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  do {
    bytes_read = subprocess_read_stdout(&process, data + index,
                                        sizeof(data) - 1 - index);
    index += bytes_read;
  } while (bytes_read != 0);

  ASSERT_EQ(212992u, index);

  for (index = 0; index < 16384; index++) {
    const char *const helloWorld = "Hello, world!";
    ASSERT_TRUE(0 == memcmp(data + (index * strlen(helloWorld)), helloWorld,
                            strlen(helloWorld)));
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, 0);
}

UTEST(subprocess, read_stdout_async_small) {
  const char *const commandLine[] = {"./process_stdout_large", "1", 0};
  struct subprocess_s process;
  int ret = -1;
  static char data[256 + 1] = {0};
  unsigned index = 0;
  unsigned bytes_read = 0;

  ASSERT_EQ(0, subprocess_create(commandLine, subprocess_option_enable_async,
                                 &process));

  do {
    bytes_read = subprocess_read_stdout(&process, data + index,
                                        sizeof(data) - 1 - index);
    index += bytes_read;
  } while (bytes_read != 0);

  ASSERT_EQ(13u, index);

  for (index = 0; index < 1; index++) {
    const char *const helloWorld = "Hello, world!";
    ASSERT_TRUE(0 == memcmp(data + (index * strlen(helloWorld)), helloWorld,
                            strlen(helloWorld)));
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, 0);
}

UTEST(subprocess, read_stdout_async) {
  const char *const commandLine[] = {"./process_stdout_large", "16384", 0};
  struct subprocess_s process;
  int ret = -1;
  static char data[1048576 + 1] = {0};
  unsigned index = 0;
  unsigned bytes_read = 0;

  ASSERT_EQ(0, subprocess_create(commandLine, subprocess_option_enable_async,
                                 &process));

  do {
    bytes_read = subprocess_read_stdout(&process, data + index,
                                        sizeof(data) - 1 - index);
    index += bytes_read;
  } while (bytes_read != 0);

  ASSERT_EQ(212992u, index);

  for (index = 0; index < 16384; index++) {
    const char *const helloWorld = "Hello, world!";
    ASSERT_TRUE(0 == memcmp(data + (index * strlen(helloWorld)), helloWorld,
                            strlen(helloWorld)));
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, 0);
}

UTEST(subprocess, poll_stdout_async) {
  const char *const commandLine[] = {"./process_stdout_poll", "16384", 0};
  struct subprocess_s process;
  int ret = -1;
  static char data[1048576 + 1] = {0};
  unsigned index = 0;
  unsigned bytes_read = 0;

  ASSERT_EQ(0, subprocess_create(commandLine, subprocess_option_enable_async,
                                 &process));

  do {
    bytes_read = subprocess_read_stdout(&process, data + index,
                                        sizeof(data) - 1 - index);

    // Send the control character to the subprocess to tell it to stop after
    // we've read at least one thing from it's stdout (meaning the read was
    // definitely async).
    if (index == 0) {
      fputc('s', subprocess_stdin(&process));
      fflush(subprocess_stdin(&process));
    }

    index += bytes_read;
  } while (bytes_read != 0);

  ASSERT_EQ(212992u, index);

  for (index = 0; index < 16384; index++) {
    const char *const helloWorld = "Hello, world!";
    ASSERT_TRUE(0 == memcmp(data + (index * strlen(helloWorld)), helloWorld,
                            strlen(helloWorld)));
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, 0);
}

UTEST(subprocess, poll_stderr_async) {
  const char *const commandLine[] = {"./process_stderr_poll", "16384", 0};
  struct subprocess_s process;
  int ret = -1;
  static char data[1048576 + 1] = {0};
  unsigned index = 0;
  unsigned bytes_read = 0;

  ASSERT_EQ(0, subprocess_create(commandLine, subprocess_option_enable_async,
                                 &process));

  do {
    bytes_read = subprocess_read_stderr(&process, data + index,
                                        sizeof(data) - 1 - index);

    // Send the control character to the subprocess to tell it to stop after
    // we've read at least one thing from it's stdout (meaning the read was
    // definitely async).
    if (index == 0) {
      fputc('s', subprocess_stdin(&process));
      fflush(subprocess_stdin(&process));
    }

    index += bytes_read;
  } while (bytes_read != 0);

  ASSERT_EQ(212992u, index);

  for (index = 0; index < 16384; index++) {
    const char *const helloWorld = "Hello, world!";
    ASSERT_TRUE(0 == memcmp(data + (index * strlen(helloWorld)), helloWorld,
                            strlen(helloWorld)));
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));
  ASSERT_EQ(0, subprocess_destroy(&process));
  ASSERT_EQ(ret, 0);
}

UTEST(create, subprocess_alive) {
  const char *const commandLine[] = {"./process_return_stdin_count", 0};
  struct subprocess_s process;
  int ret = -1;
  FILE *stdin_file;
  const char temp[41] = "Wee, sleekit, cow'rin, tim'rous beastie!";

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_NE(0, subprocess_alive(&process));

  stdin_file = subprocess_stdin(&process);
  ASSERT_TRUE(stdin_file);

  ASSERT_NE(EOF, fputs(temp, stdin_file));

  ASSERT_NE(0, subprocess_alive(&process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, subprocess_alive(&process));

  ASSERT_EQ(40, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(create, subprocess_alive_then_join) {
  const char *const commandLine[] = {"./process_return_zero", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  while (0 != subprocess_alive(&process)) {
  }

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(environment, illegal_inherit_environment) {
  const char *const commandLine[] = {"./process_return_zero", 0};
  const char *const environment[] = {"FOO=BAR", 0};
  struct subprocess_s process;

  ASSERT_NE(0, subprocess_create_ex(commandLine,
                                    subprocess_option_inherit_environment,
                                    environment, &process));
}

UTEST(environment, illegal_empty_environment_with_inherit_environment) {
  const char *const commandLine[] = {"./process_return_zero", 0};
  const char *const environment[] = {0};
  struct subprocess_s process;

  ASSERT_NE(0, subprocess_create_ex(commandLine,
                                    subprocess_option_inherit_environment,
                                    environment, &process));
}

UTEST(environment, null_environment_with_inherit_environment) {
  const char *const commandLine[] = {"./process_return_zero", 0};
  struct subprocess_s process;

  ASSERT_EQ(0, subprocess_create_ex(commandLine,
                                    subprocess_option_inherit_environment, 0,
                                    &process));

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST(environment, specify_environment) {
  const char *const commandLine[] = {"./process_inherit_environment", 0};
  const char *const environment[] = {"PROCESS_ENV_TEST=42", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create_ex(commandLine, 0, environment, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(42, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

#if !defined(_MSC_VER)
UTEST(executable_resolve, no_slashes_with_environment) {
  const char *const commandLine[] = {"process_inherit_environment", 0};
  const char *const environment[] = {"PROCESS_ENV_TEST=42", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create_ex(commandLine, 0, environment, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(42, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}
#endif

#if !defined(_MSC_VER) && !defined(__MINGW32__)
UTEST(executable_resolve, no_slashes_with_inherit) {
  const char *const commandLine[] = {"process_inherit_environment", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create_ex(commandLine,
                                    subprocess_option_inherit_environment, 0,
                                    &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(42, ret);
}
#endif

#if !defined(_MSC_VER)
UTEST(executable_resolve, custom_search_path) {
  char current_path[4096];
  char path_var[4096 + 5];
  const char *environment[2] = {0 /* placeholder */, 0};
  const char *const commandLine[] = {"process_call_return_argc", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_NE(NULL, getcwd(current_path, sizeof(current_path)));

  // Set the PATH=current_path env variable
  snprintf(path_var, sizeof(path_var), "PATH=%s", current_path);
  environment[0] = path_var;

  ASSERT_EQ(0, subprocess_create_ex(commandLine, 0, environment, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(2, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}
#endif

#if !defined(_MSC_VER) && !defined(__MINGW32__)
UTEST(executable_resolve, missing_from_path) {
  const char *const commandLine[] = {"process_call_return_argc", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine, 0, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_NE(2, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}
#endif

#if !defined(_MSC_VER)
UTEST(executable_resolve, default_search_path) {
  const char *const commandLine[] = {"ls", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(0, subprocess_create(commandLine,
                                 subprocess_option_search_user_path, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}
#endif

UTEST(create, subprocess_option_no_window) {
  const char *const commandLine[] = {"./process_return_zero", 0};
  struct subprocess_s process;
  int ret = -1;

  ASSERT_EQ(
      0, subprocess_create(commandLine, subprocess_option_no_window, &process));

  ASSERT_EQ(0, subprocess_join(&process, &ret));

  ASSERT_EQ(0, ret);

  ASSERT_EQ(0, subprocess_destroy(&process));
}

UTEST_MAIN()
