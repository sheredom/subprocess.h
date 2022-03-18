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

include(GNUInstallDirs)

foreach(name
        "process_return_zero"
        "process_return_fortytwo"
        "process_return_argc"
        "process_return_argv"
        "process_return_special_argv"
        "process_return_stdin"
        "process_return_stdin_count"
        "process_stdout_argc"
        "process_stdout_argv"
        "process_stderr_argc"
        "process_stderr_argv"
        "process_combined_stdout_stderr"
        "process_inherit_environment"
        "process_fail_divzero"
        "process_fail_stackoverflow"
        "process_hung"
        "process_stdout_data"
        "process_stdout_poll"
        "process_stderr_poll"
        "process_stdout_large")
  add_executable("${name}" "${name}.c")
  target_include_directories(
          "${name}"
          PRIVATE
          "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
  )
  #add_test(NAME "${name}" COMMAND "${name}")
endforeach()

add_executable(subprocess_test
  "utest.h"
  "main.c"
  "test.c"
  "test.cpp"
  "pre_windows.cpp"
  "post_windows.cpp"
)
target_link_libraries(subprocess_test PRIVATE "${PROJECT_NAME}")
target_include_directories(
        subprocess_test
        PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(
        subprocess_test
        PROPERTIES
        LINKER_LANGUAGE
        C
)
add_test(NAME "subprocess_test" COMMAND "subprocess_test")

#set(MSVC_FLAGS "/Wall /WX /wd4514 /wd4710 /wd4711 /wd5045")

#if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
#  set_source_files_properties(main.c test.c PROPERTIES
#    COMPILE_FLAGS "-Wall -Wextra -Werror -std=gnu89"
#  )
#elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
#  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
#    set_source_files_properties(main.c test.c PROPERTIES
#      COMPILE_FLAGS "${MSVC_FLAGS}"
#    )
#  else()
#    set_source_files_properties(main.c test.c PROPERTIES
#      COMPILE_FLAGS "-Wall -Wextra -Weverything -Werror -std=gnu89"
#    )
#  endif()
#elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
#  set_source_files_properties(main.c test.c PROPERTIES
#    COMPILE_FLAGS "${MSVC_FLAGS}"
#  )
#else()
#  message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
#endif()
#
#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#  set_source_files_properties(test.cpp PROPERTIES
#    COMPILE_FLAGS "-Wall -Wextra -Werror -Wpedantic"
#  )
#elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
#    set_source_files_properties(test.cpp PROPERTIES
#      COMPILE_FLAGS "${MSVC_FLAGS}"
#    )
#  else()
#    set_source_files_properties(test.cpp PROPERTIES
#      COMPILE_FLAGS "-Wall -Wextra -Weverything -Werror -Wpedantic"
#    )
#  endif()
#elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
#  set_source_files_properties(test.cpp PROPERTIES
#    COMPILE_FLAGS "${MSVC_FLAGS}"
#  )
#else()
#  message(WARNING "Unknown compiler '${CMAKE_C_COMPILER_ID}'!")
#endif()

add_executable(subprocess_mt_test
  "utest.h"
  "main.c"
  "test.c"
  "test.cpp"
  "pre_windows.cpp"
  "post_windows.cpp"
)
target_link_libraries(subprocess_mt_test PRIVATE "${PROJECT_NAME}")
target_include_directories(
        subprocess_mt_test
        PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(
        subprocess_mt_test
        PROPERTIES
        LINKER_LANGUAGE
        C
)
add_test(NAME "subprocess_mt_test" COMMAND "subprocess_mt_test")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(subprocess_mt_test PUBLIC "/MT")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(subprocess_mt_test PUBLIC "/MT")
endif()
