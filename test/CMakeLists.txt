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

project(process)
cmake_minimum_required(VERSION 2.8.7)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

add_executable(process_return_zero process_return_zero.c)
add_executable(process_return_fortytwo process_return_fortytwo.c)
add_executable(process_return_argc process_return_argc.c)
add_executable(process_return_argv process_return_argv.c)
add_executable(process_return_stdin process_return_stdin.c)
add_executable(process_return_stdin_count process_return_stdin_count.c)
add_executable(process_stdout_argc process_stdout_argc.c)
add_executable(process_stdout_argv process_stdout_argv.c)
add_executable(process_stderr_argc process_stderr_argc.c)
add_executable(process_stderr_argv process_stderr_argv.c)

add_executable(process_test
  ../process.h
  utest.h
  main.c
  test.c
  test.cpp
)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  set_source_files_properties(main.c test.c PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Werror -std=gnu89"
  )
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  set_source_files_properties(main.c test.c PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Weverything -Werror -std=gnu89"
  )
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
  set_source_files_properties(main.c test.c PROPERTIES
    COMPILE_FLAGS "/Wall /WX /wd4514"
  )
else()
  message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set_source_files_properties(test.cpp PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Werror -Wpedantic"
  )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set_source_files_properties(test.cpp PROPERTIES
    COMPILE_FLAGS "-Wall -Wextra -Weverything -Werror -Wpedantic"
  )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_source_files_properties(test.cpp PROPERTIES
    COMPILE_FLAGS "/Wall /WX /wd4514"
  )
else()
  message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
endif()
