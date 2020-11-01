/*
   The latest version of this library is available on GitHub;
   https://github.com/sheredom/subprocess.h
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

#ifndef SHEREDOM_SUBPROCESS_H_INCLUDED
#define SHEREDOM_SUBPROCESS_H_INCLUDED

#if defined(_MSC_VER)
#pragma warning(push, 1)
#endif

#include <stdio.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#if defined(__clang__) || defined(__GNUC__)
#define subprocess_pure __attribute__((pure))
#define subprocess_weak __attribute__((weak))
#elif defined(_MSC_VER)
#define subprocess_pure
#define subprocess_weak __inline
#else
#error Non clang, non gcc, non MSVC compiler found!
#endif

struct subprocess_s;

enum subprocess_option_e {
  // stdout and stderr are the same FILE.
  subprocess_option_combined_stdout_stderr = 0x1,

  // The child process should inherit the environment variables of the parent.
  subprocess_option_inherit_environment = 0x2,

  // Enable asynchronous reading of stdout/stderr before it has completed.
  subprocess_option_enable_async = 0x4
};

#if defined(__cplusplus)
extern "C" {
#endif

/// @brief Create a process.
/// @param command_line An array of strings for the command line to execute for
/// this process. The last element must be NULL to signify the end of the array.
/// @param options A bit field of subprocess_option_e's to pass.
/// @param out_process The newly created process.
/// @return On success 0 is returned.
subprocess_weak int subprocess_create(const char *const command_line[],
                                      int options,
                                      struct subprocess_s *const out_process);

/// @brief Get the standard input file for a process.
/// @param process The process to query.
/// @return The file for standard input of the process.
///
/// The file returned can be written to by the parent process to feed data to
/// the standard input of the process.
subprocess_pure subprocess_weak FILE *
subprocess_stdin(const struct subprocess_s *const process);

/// @brief Get the standard output file for a process.
/// @param process The process to query.
/// @return The file for standard output of the process.
///
/// The file returned can be read from by the parent process to read data from
/// the standard output of the child process.
subprocess_pure subprocess_weak FILE *
subprocess_stdout(const struct subprocess_s *const process);

/// @brief Get the standard error file for a process.
/// @param process The process to query.
/// @return The file for standard error of the process.
///
/// The file returned can be read from by the parent process to read data from
/// the standard error of the child process.
///
/// If the process was created with the subprocess_option_combined_stdout_stderr
/// option bit set, this function will return NULL, and the subprocess_stdout
/// function should be used for both the standard output and error combined.
subprocess_pure subprocess_weak FILE *
subprocess_stderr(const struct subprocess_s *const process);

/// @brief Wait for a process to finish execution.
/// @param process The process to wait for.
/// @param out_return_code The return code of the returned process (can be
/// NULL).
/// @return On success 0 is returned.
///
/// Joining a process will close the stdin pipe to the process.
subprocess_weak int subprocess_join(struct subprocess_s *const process,
                                    int *const out_return_code);

/// @brief Destroy a previously created process.
/// @param process The process to destroy.
/// @return On success 0 is returned.
///
/// If the process to be destroyed had not finished execution, it may out live
/// the parent process.
subprocess_weak int subprocess_destroy(struct subprocess_s *const process);

/// @brief Terminate a previously created process.
/// @param process The process to terminate.
/// @return On success 0 is returned.
///
/// If the process to be destroyed had not finished execution, it will be
/// terminated (i.e killed).
subprocess_weak int subprocess_terminate(struct subprocess_s *const process);

/// @brief Read the standard output from the child process.
/// @param process The process to read from.
/// @param buffer The buffer to read into.
/// @param size The maximum number of bytes to read.
/// @return The number of bytes actually read into buffer. Can only be 0 if the
/// process has complete.
///
/// The only safe way to read from the standard output of a process during it's
/// execution is to use the `subprocess_option_enable_async` option in
/// conjuction with this method.
subprocess_weak unsigned
subprocess_read_stdout(struct subprocess_s *const process, char *const buffer,
                       unsigned size);

/// @brief Read the standard error from the child process.
/// @param process The process to read from.
/// @param buffer The buffer to read into.
/// @param size The maximum number of bytes to read.
/// @return The number of bytes actually read into buffer. Can only be 0 if the
/// process has complete.
///
/// The only safe way to read from the standard error of a process during it's
/// execution is to use the `subprocess_option_enable_async` option in
/// conjuction with this method.
subprocess_weak unsigned
subprocess_read_stderr(struct subprocess_s *const process, char *const buffer,
                       unsigned size);

#if defined(__cplusplus)
#define SUBPROCESS_CAST(type, x) static_cast<type>(x)
#else
#define SUBPROCESS_CAST(type, x) ((type)x)
#endif

#if !defined(_MSC_VER)
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#endif

#if defined(_MSC_VER)
#ifdef _WIN64
typedef __int64 subprocess_intptr_t;
typedef unsigned __int64 subprocess_size_t;
#else
typedef int subprocess_intptr_t;
typedef unsigned int subprocess_size_t;
#endif

typedef struct _PROCESS_INFORMATION *LPPROCESS_INFORMATION;
typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;
typedef struct _STARTUPINFOA *LPSTARTUPINFOA;
typedef struct _OVERLAPPED *LPOVERLAPPED;

#pragma warning(push, 1)
struct subprocess_subprocess_information_s {
  void *hProcess;
  void *hThread;
  unsigned long dwProcessId;
  unsigned long dwThreadId;
};

struct subprocess_security_attributes_s {
  unsigned long nLength;
  void *lpSecurityDescriptor;
  int bInheritHandle;
};

struct subprocess_startup_info_s {
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

struct subprocess_overlapped_s {
  uintptr_t Internal;
  uintptr_t InternalHigh;
  union {
    struct {
      unsigned long Offset;
      unsigned long OffsetHigh;
    } DUMMYSTRUCTNAME;
    void *Pointer;
  } DUMMYUNIONNAME;

  void *hEvent;
};

#pragma warning(pop)

__declspec(dllimport) unsigned long __stdcall GetLastError(void);
__declspec(dllimport) int __stdcall SetHandleInformation(void *, unsigned long,
                                                         unsigned long);
__declspec(dllimport) int __stdcall CreatePipe(void **, void **,
                                               LPSECURITY_ATTRIBUTES,
                                               unsigned long);
__declspec(dllimport) void *__stdcall CreateNamedPipeA(
    const char *, unsigned long, unsigned long, unsigned long, unsigned long,
    unsigned long, unsigned long, LPSECURITY_ATTRIBUTES);
__declspec(dllimport) int __stdcall ReadFile(void *, void *, unsigned long,
                                             unsigned long *, LPOVERLAPPED);
__declspec(dllimport) unsigned long __stdcall GetCurrentProcessId(void);
__declspec(dllimport) unsigned long __stdcall GetCurrentThreadId(void);
__declspec(dllimport) void *__stdcall CreateFileA(const char *, unsigned long,
                                                  unsigned long,
                                                  LPSECURITY_ATTRIBUTES,
                                                  unsigned long, unsigned long,
                                                  void *);
__declspec(dllimport) void *__stdcall CreateEventA(LPSECURITY_ATTRIBUTES, int,
                                                   int, const char *);
__declspec(dllimport) int __stdcall CreateProcessA(
    const char *, char *, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, int,
    unsigned long, void *, const char *, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
__declspec(dllimport) int __stdcall CloseHandle(void *);
__declspec(dllimport) unsigned long __stdcall WaitForSingleObject(
    void *, unsigned long);
__declspec(dllimport) int __stdcall GetExitCodeProcess(
    void *, unsigned long *lpExitCode);
__declspec(dllimport) int __stdcall TerminateProcess(
  void *, unsigned int);
__declspec(dllimport) unsigned long __stdcall WaitForMultipleObjects(
    unsigned long, void *const *, int, unsigned long);
__declspec(dllimport) int __stdcall GetOverlappedResult(void *, LPOVERLAPPED,
                                                        unsigned long *, int);

#if defined(_DLL) && (_DLL == 1)
#define SUBPROCESS_DLLIMPORT __declspec(dllimport)
#else
#define SUBPROCESS_DLLIMPORT
#endif

SUBPROCESS_DLLIMPORT int __cdecl _fileno(FILE *);
SUBPROCESS_DLLIMPORT int __cdecl _open_osfhandle(subprocess_intptr_t, int);
SUBPROCESS_DLLIMPORT subprocess_intptr_t __cdecl _get_osfhandle(int);

void *__cdecl _alloca(subprocess_size_t);
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif
struct subprocess_s {
  FILE *stdin_file;
  FILE *stdout_file;
  FILE *stderr_file;

#if defined(_MSC_VER)
  void *hProcess;
  void *hStdInput;
  void *hEventOutput;
  void *hEventError;
#else
  pid_t child;
#endif
};
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#if defined(_MSC_VER)
subprocess_weak int subprocess_create_named_pipe_helper(void **rd, void **wr);
int subprocess_create_named_pipe_helper(void **rd, void **wr) {
  const unsigned long pipeAccessInbound = 0x00000001;
  const unsigned long fileFlagOverlapped = 0x40000000;
  const unsigned long pipeTypeByte = 0x00000000;
  const unsigned long pipeWait = 0x00000000;
  const unsigned long genericWrite = 0x40000000;
  const unsigned long openExisting = 3;
  const unsigned long fileAttributeNormal = 0x00000080;
  const void *const invalidHandleValue = (void *)~((subprocess_intptr_t)0);
  struct subprocess_security_attributes_s saAttr = {sizeof(saAttr), 0, 1};
  char name[256] = {0};
  __declspec(thread) static long index = 0;
  const long unique = index++;

#if _MSC_VER < 1900
#pragma warning(disable : 4996)
#pragma warning(push, 1)

  _snprintf(name, sizeof(name) - 1,
            "\\\\.\\pipe\\sheredom_subprocess_h.%08lx.%08lx.%ld",
            GetCurrentProcessId(), GetCurrentThreadId(), unique);

#pragma warning(pop)
#else
#pragma warning(disable : 4710)
#pragma warning(push, 1)
  snprintf(name, sizeof(name) - 1,
           "\\\\.\\pipe\\sheredom_subprocess_h.%08lx.%08lx.%ld",
           GetCurrentProcessId(), GetCurrentThreadId(), unique);
#pragma warning(pop)
#endif

  *rd = CreateNamedPipeA(name, pipeAccessInbound | fileFlagOverlapped,
                         pipeTypeByte | pipeWait, 1, 4096, 4096, 0,
                         (LPSECURITY_ATTRIBUTES)&saAttr);

  if (invalidHandleValue == rd) {
    return -1;
  }

  *wr = CreateFileA(name, genericWrite, 0, (LPSECURITY_ATTRIBUTES)&saAttr,
                    openExisting, fileAttributeNormal, 0);

  if (invalidHandleValue == wr) {
    return -1;
  }

  return 0;
}
#endif

int subprocess_create(const char *const commandLine[], int options,
                      struct subprocess_s *const out_process) {
#if defined(_MSC_VER)
  int fd;
  void *rd, *wr;
  char *commandLineCombined;
  subprocess_size_t len;
  int i, j;
  const unsigned long startFUseStdHandles = 0x00000100;
  const unsigned long handleFlagInherit = 0x00000001;
  struct subprocess_subprocess_information_s processInfo;
  struct subprocess_security_attributes_s saAttr = {sizeof(saAttr), 0, 1};
  char *environment = 0;
  struct subprocess_startup_info_s startInfo = {0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                0, 0, 0, 0, 0, 0, 0, 0, 0};

  startInfo.cb = sizeof(startInfo);
  startInfo.dwFlags = startFUseStdHandles;

  if (subprocess_option_inherit_environment !=
      (options & subprocess_option_inherit_environment)) {
    environment = "\0\0";
  }

  if (!CreatePipe(&rd, &wr, (LPSECURITY_ATTRIBUTES)&saAttr, 0)) {
    return -1;
  }

  if (!SetHandleInformation(wr, handleFlagInherit, 0)) {
    return -1;
  }

  fd = _open_osfhandle((subprocess_intptr_t)wr, 0);

  if (-1 != fd) {
    out_process->stdin_file = _fdopen(fd, "wb");

    if (0 == out_process->stdin_file) {
      return -1;
    }
  }

  startInfo.hStdInput = rd;

  if (options & subprocess_option_enable_async) {
    if (subprocess_create_named_pipe_helper(&rd, &wr)) {
      return -1;
    }
  } else {
    if (!CreatePipe(&rd, &wr, (LPSECURITY_ATTRIBUTES)&saAttr, 0)) {
      return -1;
    }
  }

  if (!SetHandleInformation(rd, handleFlagInherit, 0)) {
    return -1;
  }

  fd = _open_osfhandle((subprocess_intptr_t)rd, 0);

  if (-1 != fd) {
    out_process->stdout_file = _fdopen(fd, "rb");

    if (0 == out_process->stdout_file) {
      return -1;
    }
  }

  startInfo.hStdOutput = wr;

  if (subprocess_option_combined_stdout_stderr ==
      (options & subprocess_option_combined_stdout_stderr)) {
    out_process->stderr_file = out_process->stdout_file;
    startInfo.hStdError = startInfo.hStdOutput;
  } else {
    if (options & subprocess_option_enable_async) {
      if (subprocess_create_named_pipe_helper(&rd, &wr)) {
        return -1;
      }
    } else {
      if (!CreatePipe(&rd, &wr, (LPSECURITY_ATTRIBUTES)&saAttr, 0)) {
        return -1;
      }
    }

    if (!SetHandleInformation(rd, handleFlagInherit, 0)) {
      return -1;
    }

    fd = _open_osfhandle((subprocess_intptr_t)rd, 0);

    if (-1 != fd) {
      out_process->stderr_file = _fdopen(fd, "rb");

      if (0 == out_process->stderr_file) {
        return -1;
      }
    }

    startInfo.hStdError = wr;
  }

  if (options & subprocess_option_enable_async) {
    out_process->hEventOutput =
        CreateEventA((LPSECURITY_ATTRIBUTES)&saAttr, 1, 1, 0);
    out_process->hEventError =
        CreateEventA((LPSECURITY_ATTRIBUTES)&saAttr, 1, 1, 0);
  } else {
    out_process->hEventOutput = 0;
    out_process->hEventError = 0;
  }

  // Combine commandLine together into a single string
  len = 0;
  for (i = 0; commandLine[i]; i++) {
    // For the ' ' and two '"' between items and trailing '\0'
    len += 3;

    for (j = 0; '\0' != commandLine[i][j]; j++) {
      switch (commandLine[i][j]) {
        case '\\':
          if (commandLine[i][j + 1] != '"') break;
        case '"':
          len++;
      }
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
    commandLineCombined[len++] = '"';

    for (j = 0; '\0' != commandLine[i][j]; j++) {
      switch (commandLine[i][j]) {
        case '\\':
          if (commandLine[i][j + 1] != '"') break;
        case '"':
          commandLineCombined[len++] = '\\';
      }
      commandLineCombined[len++] = commandLine[i][j];
    }
    commandLineCombined[len++] = '"';
  }

  commandLineCombined[len] = '\0';

  if (!CreateProcessA(NULL,
                      commandLineCombined, // command line
                      NULL,                // process security attributes
                      NULL,                // primary thread security attributes
                      1,                   // handles are inherited
                      0,                   // creation flags
                      environment,         // use parent's environment
                      NULL,                // use parent's current directory
                      (LPSTARTUPINFOA)&startInfo, // STARTUPINFO pointer
                      (LPPROCESS_INFORMATION)&processInfo)) {
    return -1;
  }

  out_process->hProcess = processInfo.hProcess;

  out_process->hStdInput = startInfo.hStdInput;

  // We don't need the handle of the primary thread in the called process.
  CloseHandle(processInfo.hThread);

  if (0 != startInfo.hStdOutput) {
    CloseHandle(startInfo.hStdOutput);

    if (startInfo.hStdError != startInfo.hStdOutput) {
      CloseHandle(startInfo.hStdError);
    }
  }

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

  if (subprocess_option_combined_stdout_stderr !=
      (options & subprocess_option_combined_stdout_stderr)) {
    if (0 != pipe(stderrfd)) {
      return -1;
    }
  }

  child = fork();

  if (-1 == child) {
    return -1;
  }

  if (0 == child) {
    // Close the stdin write end
    close(stdinfd[1]);
    // Map the read end to stdin
    dup2(stdinfd[0], STDIN_FILENO);

    // Close the stdout read end
    close(stdoutfd[0]);
    // Map the write end to stdout
    dup2(stdoutfd[1], STDOUT_FILENO);

    if (subprocess_option_combined_stdout_stderr ==
        (options & subprocess_option_combined_stdout_stderr)) {
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
    if (subprocess_option_inherit_environment !=
        (options & subprocess_option_inherit_environment)) {
      char *const environment[1] = {0};
      exit(execve(commandLine[0], (char *const *)commandLine, environment));
    } else {
      exit(execvp(commandLine[0], (char *const *)commandLine));
    }
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

    if (subprocess_option_combined_stdout_stderr ==
        (options & subprocess_option_combined_stdout_stderr)) {
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

FILE *subprocess_stdin(const struct subprocess_s *const process) {
  return process->stdin_file;
}

FILE *subprocess_stdout(const struct subprocess_s *const process) {
  return process->stdout_file;
}

FILE *subprocess_stderr(const struct subprocess_s *const process) {
  if (process->stdout_file != process->stderr_file) {
    return process->stderr_file;
  } else {
    return 0;
  }
}

int subprocess_join(struct subprocess_s *const process,
                    int *const out_return_code) {
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
      *out_return_code = EXIT_FAILURE;
    }
  }

  return 0;
#endif
}

int subprocess_destroy(struct subprocess_s *const process) {
  if (0 != process->stdin_file) {
    fclose(process->stdin_file);
    process->stdin_file = 0;
  }

  if (0 != process->stdout_file) {
    fclose(process->stdout_file);

    if (process->stdout_file != process->stderr_file) {
      fclose(process->stderr_file);
    }

    process->stdout_file = 0;
    process->stderr_file = 0;
  }

#if defined(_MSC_VER)
  if (process->hProcess) {
    CloseHandle(process->hProcess);
    process->hProcess = 0;

    if (0 != process->hStdInput) {
      CloseHandle(process->hStdInput);
    }

    if (0 != process->hEventOutput) {
      CloseHandle(process->hEventOutput);
    }

    if (0 != process->hEventError) {
      CloseHandle(process->hEventError);
    }
  }
#endif

  return 0;
}

int subprocess_terminate(struct subprocess_s *const process) {
#if defined(_MSC_VER)
  unsigned int killed_process_exit_code;
  int success_terminate;
  int windows_call_result;
  
  killed_process_exit_code = 99;
  windows_call_result = TerminateProcess(process->hProcess, killed_process_exit_code);
  success_terminate = (windows_call_result== 0) ? 1 : 0;
  return success_terminate;
#else
  int result;
  result = kill(process->child, 9);
  return result;
#endif
}

unsigned subprocess_read_stdout(struct subprocess_s *const process,
                                char *const buffer, unsigned size) {
#if defined(_MSC_VER)
  void *handle;
  unsigned long bytes_read = 0;
  struct subprocess_overlapped_s overlapped = {0};
  overlapped.hEvent = process->hEventOutput;

  handle = (void *)_get_osfhandle(_fileno(process->stdout_file));

  if (!ReadFile(handle, buffer, size, &bytes_read, (LPOVERLAPPED)&overlapped)) {
    const unsigned long errorIoPending = 997;
    unsigned long error = GetLastError();

    // Means we've got an async read!
    if (error == errorIoPending) {
      if (!GetOverlappedResult(handle, (LPOVERLAPPED)&overlapped, &bytes_read,
                               1)) {
        const unsigned long errorIoIncomplete = 996;
        const unsigned long errorHandleEOF = 38;
        error = GetLastError();

        if ((error != errorIoIncomplete) && (error != errorHandleEOF)) {
          return 0;
        }
      }
    }
  }

  return (unsigned)bytes_read;
#else
  const int fd = fileno(process->stdout_file);
  const ssize_t bytes_read = read(fd, buffer, size);

  if (bytes_read < 0) {
    return 0;
  }

  return SUBPROCESS_CAST(unsigned, bytes_read);
#endif
}

unsigned subprocess_read_stderr(struct subprocess_s *const process,
                                char *const buffer, unsigned size) {
#if defined(_MSC_VER)
  void *handle;
  unsigned long bytes_read = 0;
  struct subprocess_overlapped_s overlapped = {0};
  overlapped.hEvent = process->hEventError;

  handle = (void *)_get_osfhandle(_fileno(process->stderr_file));

  if (!ReadFile(handle, buffer, size, &bytes_read, (LPOVERLAPPED)&overlapped)) {
    const unsigned long errorIoPending = 997;
    unsigned long error = GetLastError();

    // Means we've got an async read!
    if (error == errorIoPending) {
      if (!GetOverlappedResult(handle, (LPOVERLAPPED)&overlapped, &bytes_read,
                               1)) {
        const unsigned long errorIoIncomplete = 996;
        const unsigned long errorHandleEOF = 38;
        error = GetLastError();

        if ((error != errorIoIncomplete) && (error != errorHandleEOF)) {
          return 0;
        }
      }
    }
  }

  return (unsigned)bytes_read;
#else
  const int fd = fileno(process->stderr_file);
  const ssize_t bytes_read = read(fd, buffer, size);

  if (bytes_read < 0) {
    return 0;
  }

  return SUBPROCESS_CAST(unsigned, bytes_read);
#endif
}

#if defined(__cplusplus)
} // extern "C"
#endif

#endif /* SHEREDOM_SUBPROCESS_H_INCLUDED */
