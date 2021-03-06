project (fcactus)

cmake_minimum_required (VERSION 2.6)

####################################

# Computes the realtionship between two version strings.  A version
# string is a number delineated by '.'s such as 1.3.2 and 0.99.9.1.
# You can feed version strings with different number of dot versions,
# and the shorter version number will be padded with zeros: 9.2 <
# 9.2.1 will actually compare 9.2.0 < 9.2.1.
#
# Input: a_in - value, not variable
#        b_in - value, not variable
#        result_out - variable with value:
#                         -1 : a_in <  b_in
#                          0 : a_in == b_in
#                          1 : a_in >  b_in
#
# Written by James Bigler.
MACRO(COMPARE_VERSION_STRINGS a_in b_in result_out)
  # Since SEPARATE_ARGUMENTS using ' ' as the separation token,
  # replace '.' with ' ' to allow easy tokenization of the string.
  STRING(REPLACE "." " " a ${a_in})
  STRING(REPLACE "." " " b ${b_in})
  SEPARATE_ARGUMENTS(a)
  SEPARATE_ARGUMENTS(b)

  # Check the size of each list to see if they are equal.
  LIST(LENGTH a a_length)
  LIST(LENGTH b b_length)

  # Pad the shorter list with zeros.

  # Note that range needs to be one less than the length as the for
  # loop is inclusive (silly CMake).
  IF(a_length LESS b_length)
    # a is shorter
    SET(shorter a)
    MATH(EXPR range "${b_length} - 1")
    MATH(EXPR pad_range "${b_length} - ${a_length} - 1")
  ELSE(a_length LESS b_length)
    # b is shorter
    SET(shorter b)
    MATH(EXPR range "${a_length} - 1")
    MATH(EXPR pad_range "${a_length} - ${b_length} - 1")
  ENDIF(a_length LESS b_length)

  # PAD out if we need to
  IF(NOT pad_range LESS 0)
    FOREACH(pad RANGE ${pad_range})
      # Since shorter is an alias for b, we need to get to it by by dereferencing shorter.
      LIST(APPEND ${shorter} 0)
    ENDFOREACH(pad RANGE ${pad_range})
  ENDIF(NOT pad_range LESS 0)

  SET(result 0)
  FOREACH(index RANGE ${range})
    IF(result EQUAL 0)
      # Only continue to compare things as long as they are equal
      LIST(GET a ${index} a_version)
      LIST(GET b ${index} b_version)
      # LESS
      IF(a_version LESS b_version)
        SET(result -1)
      ENDIF(a_version LESS b_version)
      # GREATER
      IF(a_version GREATER b_version)
        SET(result 1)
      ENDIF(a_version GREATER b_version)
    ENDIF(result EQUAL 0)
  ENDFOREACH(index)

  # Copy out the return result
  SET(${result_out} ${result})
ENDMACRO(COMPARE_VERSION_STRINGS)

####################################

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -std=gnu99 -fno-strict-overflow -Wall -Wextra -Wimplicit-fallthrough=0 -Wno-maybe-uninitialized -Wformat=2 -Wformat-nonliteral -Wformat-security -Wpointer-arith -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=gnu++14 -fno-strict-overflow -Wall -Wextra -Wimplicit-fallthrough=0 -Wno-maybe-uninitialized -Wformat-security -Wpointer-arith -Wno-unused-parameter")

if (WIN32)
  set(OSNAME "Win32")
else()
execute_process(
  COMMAND uname
  COMMAND tr "\n" " "
  COMMAND sed "s/ //"
  OUTPUT_VARIABLE OSNAME
  )
endif()

if (${OSNAME} STREQUAL "Linux")
  set(GLIBC_DETECT_TEST_C
    "
    #include <stdio.h>
    int main(int argc, char *argv[]) {
    printf(\"%d.%d\", __GLIBC__, __GLIBC_MINOR__);
    return 0;
    }
    ")
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/detect/glibc.c "${GLIBC_DETECT_TEST_C}\n")
  try_run(GLIBC_DETECTION GLIBC_COMPILE
    ${CMAKE_CURRENT_BINARY_DIR}/detect ${CMAKE_CURRENT_BINARY_DIR}/detect/glibc.c
    RUN_OUTPUT_VARIABLE GLIBC_VERSION)
  if (GLIBC_VERSION AND GLIBC_COMPILE)
    message("glibc version detected as ${GLIBC_VERSION}")
  else()
    message("Could not detect glibc version.")
  endif()

  COMPARE_VERSION_STRINGS(${GLIBC_VERSION} "2.17" GLIBC_MUST_LINK_RT)
  if (${GLIBC_MUST_LINK_RT} LESS 0)
    message("glibc <2.17 detected; linking against librt.")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lrt")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt")
  endif()
endif()

include_directories("${PROJECT_SOURCE_DIR}"
                    "${PROJECT_SOURCE_DIR}/ncmlib"
                    "${PROJECT_SOURCE_DIR}/fmt")
add_subdirectory(ncmlib)
add_subdirectory(fmt)

set(RAGEL_CFG ${CMAKE_CURRENT_BINARY_DIR}/cfg.cpp)

find_program(RAGEL ragel)
add_custom_command(
  OUTPUT ${RAGEL_CFG}
  COMMAND ${RAGEL} -G2 -o ${RAGEL_CFG} cfg.rl
  DEPENDS cfg.rl
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Compiling Ragel state machine: cfg.rl"
  VERBATIM
  )

file(GLOB FCACTUS_SRCS "*.cpp")
add_executable(fcactus ${RAGEL_CFG} ${FCACTUS_SRCS})
target_link_libraries(fcactus ncmlib fmt)
