/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/process.h
*/

/*
   This is free and unencumbered software released into the public domain.

   Anyone is free to copy, modify, publish, use, compile, sell, or
   distribute this software, either in source code form or as a compiled
   binary, for any purpose, commercial or non-commercial, and by any
   means.

   In jurisdictions that recognize copyright laws, the author or authors
   of this software dedicate any and all copyright interest in the
   software to the public domain. We make this dedication for the benefit
   of the public at large and to the detriment of our heirs and
   successors. We intend this dedication to be an overt act of
   relinquishment in perpetuity of all present and future rights to this
   software under copyright law.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   For more information, please refer to <http://unlicense.org/>
*/

#ifndef SHEREDOM_PROCESS_H_INCLUDED
#define SHEREDOM_PROCESS_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(push, 1)
#endif

#include <stdio.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#if !defined(_MSC_VER)
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#endif

#if defined(_MSC_VER)
#ifdef _WIN64
typedef __int64 intptr_t;
typedef unsigned __int64 size_t;
#else
typedef int intptr_t;
typedef unsigned int size_t;
#endif

typedef struct _PROCESS_INFORMATION *LPPROCESS_INFORMATION;
typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;
typedef struct _STARTUPINFOA *LPSTARTUPINFOA;

#pragma warning(push, 1)
struct process_process_information_s {
  void *hProcess;
  void *hThread;
  unsigned long dwProcessId;
  unsigned long dwThreadId;
};

struct process_security_attributes_s {
  unsigned long nLength;
  void *lpSecurityDescriptor;
  int bInheritHandle;
};

struct process_startup_info_s {
  unsigned long cb;
  char *lpReserved;
  char *lpDesktop;
  char *lpTitle;
  unsigned long dwX;
  unsigned long dwY;
  unsigned long dwXSize;
  unsigned long dwYSize;
  unsigned long dwXCountChars;
  unsigned long dwYCountChars;
  unsigned long dwFillAttribute;
  unsigned long dwFlags;
  unsigned short wShowWindow;
  unsigned short cbReserved2;
  unsigned char *lpReserved2;
  void *hStdInput;
  void *hStdOutput;
  void *hStdError;
};
#pragma warning(pop)

__declspec(dllimport) int __stdcall SetHandleInformation(void *, unsigned long,
                                                         unsigned long);
__declspec(dllimport) int __stdcall CreatePipe(void **, void **,
                                               LPSECURITY_ATTRIBUTES,
                                               unsigned long);
__declspec(dllimport) int __stdcall CreateProcessA(
    const char *, char *, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, int,
    unsigned long, void *, const char *, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
__declspec(dllimport) int __stdcall CloseHandle(void *);
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(
    void *, unsigned long);
__declspec(dllimport) int __stdcall GetExitCodeProcess(
    void *, unsigned long *lpExitCode);
__declspec(dllimport) int __cdecl _open_osfhandle(intptr_t, int);
void *__cdecl _alloca(size_t);
#endif

#if defined(__clang__) || defined(__GNUC__)
#define process_pure __attribute__((pure))
#define process_weak __attribute__((weak))
#elif defined(_MSC_VER)
#define process_pure
#define process_weak __inline
#else
#error Non clang, non gcc, non MSVC compiler found!
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif
struct process_s {
  FILE *stdin_file;
  FILE *stdout_file;
  FILE *stderr_file;

#if defined(_MSC_VER)
  void *hProcess;
  void *hStdInput;
  void *hStdOutput;
  void *hStdError;
#else
  pid_t child;
#endif
};
#ifdef __clang__
#pragma clang diagnostic pop
#endif

enum process_option_e {
  // stdout and stderr are the same FILE.
  process_option_combined_stdout_stderr = 0x1,

  // The child process should inherit the environment variables of the parent.
  process_option_inherit_environment = 0x2,
  
  // The child process will be detached from parent and may live longer.
  process_option_child_detached = 0x4
};

/// @brief Create a process.
/// @param command_line An array of strings for the command line to execute for
/// this process. The last element must be NULL to signify the end of the array.
/// @param options A bit field of process_option_e's to pass.
/// @param out_process The newly created process.
/// @return On success 0 is returned.
process_weak int process_create(const char *const command_line[], int options,
                                struct process_s *const out_process);

/// @brief Get the standard input file for a process.
/// @param process The process to query.
/// @return The file for standard input of the process.
///
/// The file returned can be written to by the parent process to feed data to
/// the standard input of the process.
process_pure process_weak FILE *
process_stdin(const struct process_s *const process);

/// @brief Get the standard output file for a process.
/// @param process The process to query.
/// @return The file for standard output of the process.
///
/// The file returned can be read from by the parent process to read data from
/// the standard output of the child process.
process_pure process_weak FILE *
process_stdout(const struct process_s *const process);

/// @brief Get the standard error file for a process.
/// @param process The process to query.
/// @return The file for standard error of the process.
///
/// The file returned can be read from by the parent process to read data from
/// the standard error of the child process.
///
/// If the process was created with the process_option_combined_stdout_stderr
/// option bit set, this function will return NULL, and the process_stdout
/// function should be used for both the standard output and error combined.
process_pure process_weak FILE *
process_stderr(const struct process_s *const process);

/// @brief Wait for a process to finish execution.
/// @param process The process to wait for.
/// @param out_return_code The return code of the returned process (can be
/// NULL).
/// @return On success 0 is returned.
process_weak int process_join(struct process_s *const process,
                              int *const out_return_code);

/// @brief Destroy a previously created process.
/// @param process The process to destroy.
///
/// If the process to be destroyed had not finished execution, it may out live
/// the parent process.
process_weak int process_destroy(struct process_s *const process);

int process_create(const char *const commandLine[], int options,
                   struct process_s *const out_process) {
#if defined(_MSC_VER)
  int fd;
  void *rd, *wr;
  char *commandLineCombined;
  size_t len;
  int i, j;
  const unsigned long startFUseStdHandles = 0x00000100;
  const unsigned long handleFlagInherit = 0x00000001;
  struct process_process_information_s processInfo;
  struct process_security_attributes_s saAttr = {sizeof(saAttr), 0, 1};
  char *environment = 0;
  struct process_startup_info_s startInfo = {0, 0, 0, 0, 0, 0, 0, 0, 0,
                                             0, 0, 0, 0, 0, 0, 0, 0, 0};

  startInfo.cb = sizeof(startInfo);
  startInfo.dwFlags = startFUseStdHandles;

  if (process_option_inherit_environment !=
      (options & process_option_inherit_environment)) {
    environment = "\0\0";
  }

  if (!CreatePipe(&rd, &wr, (LPSECURITY_ATTRIBUTES)&saAttr, 0)) {
    return -1;
  }

  if (!SetHandleInformation(wr, handleFlagInherit, 0)) {
    return -1;
  }

  fd = _open_osfhandle((intptr_t)wr, 0);

  if (-1 != fd) {
    out_process->stdin_file = _fdopen(fd, "wb");

    if (0 == out_process->stdin_file) {
      return -1;
    }
  }

  startInfo.hStdInput = rd;

  if (!CreatePipe(&rd, &wr, (LPSECURITY_ATTRIBUTES)&saAttr, 0)) {
    return -1;
  }

  if (!SetHandleInformation(rd, handleFlagInherit, 0)) {
    return -1;
  }

  fd = _open_osfhandle((intptr_t)rd, 0);

  if (-1 != fd) {
    out_process->stdout_file = _fdopen(fd, "rb");

    if (0 == out_process->stdout_file) {
      return -1;
    }
  }

  startInfo.hStdOutput = wr;

  if (process_option_combined_stdout_stderr ==
      (options & process_option_combined_stdout_stderr)) {
    out_process->stderr_file = out_process->stdout_file;
    startInfo.hStdError = startInfo.hStdOutput;
  } else {
    if (!CreatePipe(&rd, &wr, (LPSECURITY_ATTRIBUTES)&saAttr, 0)) {
      return -1;
    }

    if (!SetHandleInformation(rd, handleFlagInherit, 0)) {
      return -1;
    }

    fd = _open_osfhandle((intptr_t)rd, 0);

    if (-1 != fd) {
      out_process->stderr_file = _fdopen(fd, "rb");

      if (0 == out_process->stderr_file) {
        return -1;
      }
    }

    startInfo.hStdError = wr;
  }

  // Combine commandLine together into a single string
  len = 0;
  for (i = 0; commandLine[i]; i++) {
    // For the ' ' between items and trailing '\0'
    len++;

    for (j = 0; '\0' != commandLine[i][j]; j++) {
      len++;
    }
  }

  commandLineCombined = (char *)_alloca(len);

  if (!commandLineCombined) {
    return -1;
  }

  // Gonna re-use len to store the write index into commandLineCombined
  len = 0;

  for (i = 0; commandLine[i]; i++) {
    if (0 != i) {
      commandLineCombined[len++] = ' ';
    }

    for (j = 0; '\0' != commandLine[i][j]; j++) {
      commandLineCombined[len++] = commandLine[i][j];
    }
  }

  commandLineCombined[len] = '\0';

  unsigned long flags = 0x0;
  if (process_option_child_detached ==
      (options & process_option_child_detached)) {
    flags |= 0x00000008; /* DETACHED_PROCESS */
  }
  if (!CreateProcessA(NULL,
                      commandLineCombined, // command line
                      NULL,                // process security attributes
                      NULL,                // primary thread security attributes
                      1,                   // handles are inherited
                      flags,               // creation flags
                      environment,         // use parent's environment
                      NULL,                // use parent's current directory
                      (LPSTARTUPINFOA)&startInfo, // STARTUPINFO pointer
                      (LPPROCESS_INFORMATION)&processInfo)) {
    return -1;
  }
  if (!(process_option_child_detached ==
      (options & process_option_child_detached))) {
    out_process->hProcess = processInfo.hProcess;

    out_process->hStdInput = startInfo.hStdInput;
    out_process->hStdOutput = startInfo.hStdOutput;
    out_process->hStdError = startInfo.hStdError;
  } else {
    CloseHandle(processInfo.hProcess);
  }
  
  // We don't need the handle of the primary thread in the called process.
  CloseHandle(processInfo.hThread);

  return 0;
#else
  int stdinfd[2];
  int stdoutfd[2];
  int stderrfd[2];
  pid_t child;

  if (0 != pipe(stdinfd)) {
    return -1;
  }

  if (0 != pipe(stdoutfd)) {
    return -1;
  }

  if (process_option_combined_stdout_stderr !=
      (options & process_option_combined_stdout_stderr)) {
    if (0 != pipe(stderrfd)) {
      return -1;
    }
  }

  child = fork();

  if (-1 == child) {
    return -1;
  }

  if (0 == child) {
    if (process_option_child_detached ==
        (options & process_option_child_detached)) {
      struct sigaction noaction;
      memset(&noaction, 0, sizeof(noaction));
      noaction.sa_handler = SIG_IGN;
      sigaction(SIGPIPE, &noaction, 0);
      setsid();
    }
    // Close the stdin write end
    close(stdinfd[1]);
    // Map the read end to stdin
    dup2(stdinfd[0], STDIN_FILENO);

    // Close the stdout read end
    close(stdoutfd[0]);
    // Map the write end to stdout
    dup2(stdoutfd[1], STDOUT_FILENO);

    if (process_option_combined_stdout_stderr ==
        (options & process_option_combined_stdout_stderr)) {
      dup2(STDOUT_FILENO, STDERR_FILENO);
    } else {
      // Close the stderr read end
      close(stderrfd[0]);
      // Map the write end to stdout
      dup2(stderrfd[1], STDERR_FILENO);
    }

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
#pragma clang diagnostic ignored "-Wold-style-cast"
#endif
    int r;
    if (process_option_inherit_environment !=
        (options & process_option_inherit_environment)) {
      char *const environment[1] = {0};
      r = execve(commandLine[0], (char *const *)commandLine, environment);
    } else {
      r = execvp(commandLine[0], (char *const *)commandLine);
    }
    if (process_option_child_detached ==
        (options & process_option_child_detached)) {
      struct sigaction noaction;
      memset(&noaction, 0, sizeof(noaction));
      noaction.sa_handler = SIG_IGN;
      sigaction(SIGPIPE, &noaction, 0);
    }
    exit(r);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  } else {
    // Close the stdin read end
    close(stdinfd[0]);
    // Store the stdin write end
    out_process->stdin_file = fdopen(stdinfd[1], "wb");

    // Close the stdout write end
    close(stdoutfd[1]);
    // Store the stdout read end
    out_process->stdout_file = fdopen(stdoutfd[0], "rb");

    if (process_option_combined_stdout_stderr ==
        (options & process_option_combined_stdout_stderr)) {
      out_process->stderr_file = out_process->stdout_file;
    } else {
      // Close the stderr write end
      close(stderrfd[1]);
      // Store the stderr read end
      out_process->stderr_file = fdopen(stderrfd[0], "rb");
    }

    // Store the child's pid
    out_process->child = child;

    return 0;
  }
#endif
}

FILE *process_stdin(const struct process_s *const process) {
  return process->stdin_file;
}

FILE *process_stdout(const struct process_s *const process) {
  return process->stdout_file;
}

FILE *process_stderr(const struct process_s *const process) {
  if (process->stdout_file != process->stderr_file) {
    return process->stderr_file;
  } else {
    return 0;
  }
}

int process_join(struct process_s *const process, int *const out_return_code) {
#if defined(_MSC_VER)
  const unsigned long infinite = 0xFFFFFFFF;

  if (0 != process->stdin_file) {
    fclose(process->stdin_file);
    process->stdin_file = 0;
  }
  if (0 != process->hStdInput) {
    CloseHandle(process->hStdInput);
    process->hStdInput = NULL;
  }

  WaitForSingleObject(process->hProcess, infinite);

  if (0 != process->hStdOutput) {
    CloseHandle(process->hStdOutput);
    if (process->hStdError == process->hStdOutput) {
      process->hStdError = NULL;
    }
    process->hStdOutput = NULL;
  }
  if (0 != process->hStdError) {
    CloseHandle(process->hStdError);
    process->hStdError = NULL;
  }

  if (out_return_code) {
    if (!GetExitCodeProcess(process->hProcess,
                            (unsigned long *)out_return_code)) {
      return -1;
    }
  }

  return 0;
#else
  int status;

  if (0 != process->stdin_file) {
    fclose(process->stdin_file);
    process->stdin_file = 0;
  }

  if (process->child != waitpid(process->child, &status, 0)) {
    return -1;
  }

  if (out_return_code) {
    if (WIFEXITED(status)) {
      *out_return_code = WEXITSTATUS(status);
    } else {
      *out_return_code = 0;
    }
  }

  return 0;
#endif
}

int process_destroy(struct process_s *const process) {
  if (0 != process->stdin_file) {
    fclose(process->stdin_file);
  }

  fclose(process->stdout_file);

  if (process->stdout_file != process->stderr_file) {
    fclose(process->stderr_file);
  }

#if defined(_MSC_VER)
  CloseHandle(process->hProcess);

  if (0 != process->hStdInput) {
    CloseHandle(process->hStdInput);
    process->hStdInput = NULL;
  }
  if (0 != process->hStdOutput) {
    CloseHandle(process->hStdOutput);
    if (process->hStdError == process->hStdOutput) {
      process->hStdError = NULL;
    }
    process->hStdOutput = NULL;
  }
  if (0 != process->hStdError) {
    CloseHandle(process->hStdError);
    process->hStdError = NULL;
  }
#endif

  return 0;
}

#if defined(__cplusplus)
} // extern "C"
#endif

#endif /* SHEREDOM_PROCESS_H_INCLUDED */
