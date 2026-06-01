# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.
#
# In jurisdictions that recognize copyright laws, the author or authors
# of this software dedicate any and all copyright interest in the
# software to the public domain. We make this dedication for the benefit
# of the public at large and to the detriment of our heirs and
# successors. We intend this dedication to be an overt act of
# relinquishment in perpetuity of all present and future rights to this
# software under copyright law.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# For more information, please refer to <http://unlicense.org/>

cmake_minimum_required(VERSION 3.15)
project(subprocess)

include(CTest)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

set(SUBPROCESS_HELPER_SOURCES
  process_return_zero.c
  process_return_fortytwo.c
  process_return_argc.c
  process_return_argv.c
  process_return_special_argv.c
  process_return_lpcmdline.c
  process_return_stdin.c
  process_return_stdin_count.c
  process_stdout_argc.c
  process_stdout_argv.c
  process_stderr_argc.c
  process_stderr_argv.c
  process_combined_stdout_stderr.c
  process_inherit_environment.c
  process_fail_divzero.c
  process_fail_stackoverflow.c
  process_hung.c
  process_stdout_data.c
  process_stdout_poll.c
  process_stdout_poll_wait_first.c
  process_stderr_poll.c
  process_stderr_poll_wait_first.c
  process_stdout_large.c
  process_call_return_argc.c
  process_cwd.c
)

foreach(SUBPROCESS_HELPER_SOURCE ${SUBPROCESS_HELPER_SOURCES})
  get_filename_component(SUBPROCESS_HELPER_TARGET ${SUBPROCESS_HELPER_SOURCE} NAME_WE)
  add_executable(${SUBPROCESS_HELPER_TARGET} ${SUBPROCESS_HELPER_SOURCE})
endforeach()

# Base warning flags and whether -std= flags are supported.
# MSVC and clang-cl use /std: and don't accept -std=, so we skip
# the standard flag for those compilers.
set(SUBPROCESS_MSVC_WARNINGS "/Wall /WX /wd4514 /wd4710 /wd4711 /wd5045")
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  set(SUBPROCESS_C_WARNINGS "-Wall -Wextra -Werror")
  set(SUBPROCESS_C_USE_STD_FLAG TRUE)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
  if(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(SUBPROCESS_C_WARNINGS "${SUBPROCESS_MSVC_WARNINGS}")
    set(SUBPROCESS_C_USE_STD_FLAG FALSE)
  else()
    set(SUBPROCESS_C_WARNINGS "-Wall -Wextra -Weverything -Werror -Wno-poison-system-directories")
    set(SUBPROCESS_C_USE_STD_FLAG TRUE)
  endif()
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  set(SUBPROCESS_C_WARNINGS "${SUBPROCESS_MSVC_WARNINGS}")
  set(SUBPROCESS_C_USE_STD_FLAG FALSE)
else()
  message(WARNING "Unknown C compiler '${CMAKE_C_COMPILER_ID}'!")
  set(SUBPROCESS_C_WARNINGS "")
  set(SUBPROCESS_C_USE_STD_FLAG FALSE)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(SUBPROCESS_CXX_WARNINGS "-Wall -Wextra -Werror -Wpedantic")
  set(SUBPROCESS_CXX_USE_STD_FLAG TRUE)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(SUBPROCESS_CXX_WARNINGS "${SUBPROCESS_MSVC_WARNINGS}")
    set(SUBPROCESS_CXX_USE_STD_FLAG FALSE)
  else()
    set(SUBPROCESS_CXX_WARNINGS "-Wall -Wextra -Weverything -Werror -Wpedantic -Wno-poison-system-directories -Wno-format -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-c++11-compat-deprecated-writable-strings -Wno-null-arithmetic -Wno-writable-strings -Wno-undefined-func-template")
    set(SUBPROCESS_CXX_USE_STD_FLAG TRUE)
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(SUBPROCESS_CXX_WARNINGS "${SUBPROCESS_MSVC_WARNINGS}")
  set(SUBPROCESS_CXX_USE_STD_FLAG FALSE)
else()
  message(WARNING "Unknown C++ compiler '${CMAKE_CXX_COMPILER_ID}'!")
  set(SUBPROCESS_CXX_WARNINGS "")
  set(SUBPROCESS_CXX_USE_STD_FLAG FALSE)
endif()

# Helper: set compile flags for a C source file (warnings + standard).
function(subprocess_set_c_flags FILE STANDARD)
  if(SUBPROCESS_C_USE_STD_FLAG)
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${SUBPROCESS_C_WARNINGS} ${STANDARD}")
  else()
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${SUBPROCESS_C_WARNINGS}")
  endif()
endfunction()

# Helper: set compile flags for a C++ source file (warnings + standard + extras).
function(subprocess_set_cxx_flags FILE STANDARD)
  set(EXTRA_FLAGS ${ARGN})
  if(SUBPROCESS_CXX_USE_STD_FLAG)
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${SUBPROCESS_CXX_WARNINGS} ${STANDARD} ${EXTRA_FLAGS}")
  else()
    set_source_files_properties(${FILE} PROPERTIES
      COMPILE_FLAGS "${SUBPROCESS_CXX_WARNINGS} ${EXTRA_FLAGS}")
  endif()
endfunction()

# C files — each compiled under a specific C standard.
subprocess_set_c_flags(main.c  "-std=gnu89")
subprocess_set_c_flags(test.c  "-std=gnu89")
subprocess_set_c_flags(test99.c "-std=c99")

list(FIND CMAKE_C_COMPILE_FEATURES c_std_11 IDX)
if(${IDX} GREATER -1)
  subprocess_set_c_flags(test11.c "-std=c11")
endif()

# C++ files — each compiled under a specific C++ standard.
subprocess_set_cxx_flags(test.cpp   "" "-Wno-c++98-compat")
subprocess_set_cxx_flags(test98.cpp "-std=gnu++98")
subprocess_set_cxx_flags(test11.cpp "-std=c++11" "-Wno-c++98-compat")
subprocess_set_cxx_flags(test14.cpp "-std=c++14" "-Wno-c++98-compat")
subprocess_set_cxx_flags(test17.cpp "-std=c++17" "-Wno-c++98-compat")
subprocess_set_cxx_flags(test20.cpp "-std=c++20" "-Wno-c++98-compat")
subprocess_set_cxx_flags(pre_windows.cpp  "" "-Wno-c++98-compat")
subprocess_set_cxx_flags(post_windows.cpp "" "-Wno-c++98-compat")

set(SUBPROCESS_TEST_SOURCES
  ../subprocess.h
  utest.h
  test_shared.h
  main.c
  test.c
  test99.c
  test11.c
  test.cpp
  test98.cpp
  test11.cpp
  test14.cpp
  test17.cpp
  test20.cpp
  pre_windows.cpp
  post_windows.cpp
)

# Helper: add a test executable and register it with CTest.
function(subprocess_add_test NAME)
  add_executable(${NAME} ${SUBPROCESS_TEST_SOURCES})
  add_test(NAME ${NAME} COMMAND $<TARGET_FILE:${NAME}>)
  set_tests_properties(${NAME} PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:${NAME}>)
endfunction()

subprocess_add_test(subprocess_test)

if(MSVC)
  # Build a second test binary linked against the static CRT (/MT)
  # to ensure we work with both static and dynamic libc linkage.
  set_property(TARGET subprocess_test PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  subprocess_add_test(subprocess_mt_test)
  set_property(TARGET subprocess_mt_test PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Some sanitizer runtimes can't initialise on every host even when
# the toolchain links them fine — e.g. clang's TSan and MSan on
# aarch64 only ship shadow-memory layouts for 39-, 42- and 48-bit
# user VMAs, but Raspberry Pi OS' kernel is configured with
# CONFIG_ARM64_VA_BITS=47. The binary builds, but aborts before
# main with "FATAL: ThreadSanitizer: unsupported VMA range". Probe
# each candidate sanitizer with try_run and skip the ones whose
# runtime can't start. Result is cached so reconfigure is cheap;
# delete CMakeCache.txt to re-probe (e.g. after changing kernels).
function(subprocess_check_sanitizer_runs SAN OUT_VAR)
  set(CACHE_VAR "SUBPROCESS_SAN_${SAN}_RUNS")
  if(DEFINED ${CACHE_VAR})
    set(${OUT_VAR} ${${CACHE_VAR}} PARENT_SCOPE)
    return()
  endif()

  set(PROBE_SRC "${CMAKE_BINARY_DIR}/subprocess_san_probe_${SAN}.c")
  file(WRITE "${PROBE_SRC}" "int main(void) { return 0; }\n")

  try_run(RUN_RESULT COMPILE_RESULT
    "${CMAKE_BINARY_DIR}/subprocess_san_probe_${SAN}"
    "${PROBE_SRC}"
    COMPILE_DEFINITIONS "-fsanitize=${SAN}" "-fno-omit-frame-pointer"
    LINK_OPTIONS "-fsanitize=${SAN}"
  )

  if(COMPILE_RESULT AND "${RUN_RESULT}" STREQUAL "0")
    set(RESULT TRUE)
    message(STATUS "Sanitizer ${SAN}: usable")
  else()
    set(RESULT FALSE)
    message(STATUS "Sanitizer ${SAN}: skipping "
                   "(compile=${COMPILE_RESULT}, run=${RUN_RESULT})")
  endif()

  set(${CACHE_VAR} ${RESULT} CACHE INTERNAL
      "Whether the ${SAN} sanitizer runtime starts on this host")
  set(${OUT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

if(("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang") AND NOT "${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "MSVC")
  set(SUBPROCESS_SANITIZER_CANDIDATES
    address
    undefined
    thread
  )

  if(NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
    set(SUBPROCESS_SANITIZER_CANDIDATES ${SUBPROCESS_SANITIZER_CANDIDATES} memory)
  endif()

  set(SUBPROCESS_SANITIZERS)
  foreach(SUBPROCESS_SANITIZER ${SUBPROCESS_SANITIZER_CANDIDATES})
    if(CMAKE_CROSSCOMPILING)
      list(APPEND SUBPROCESS_SANITIZERS ${SUBPROCESS_SANITIZER})
    else()
      subprocess_check_sanitizer_runs(${SUBPROCESS_SANITIZER} SUBPROCESS_SAN_RUNS)
      if(SUBPROCESS_SAN_RUNS)
        list(APPEND SUBPROCESS_SANITIZERS ${SUBPROCESS_SANITIZER})
      endif()
    endif()
  endforeach()

  foreach(SUBPROCESS_SANITIZER ${SUBPROCESS_SANITIZERS})
    subprocess_add_test(subprocess_${SUBPROCESS_SANITIZER}_test)
    target_compile_options(subprocess_${SUBPROCESS_SANITIZER}_test PRIVATE -fsanitize=${SUBPROCESS_SANITIZER} -fno-omit-frame-pointer)
    target_link_options(subprocess_${SUBPROCESS_SANITIZER}_test PRIVATE -fsanitize=${SUBPROCESS_SANITIZER})
  endforeach()
endif()
