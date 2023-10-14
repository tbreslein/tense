# ===============================================================================
#
# CMake file configuring tense
#
# Copyright (c) 2023 Tommy Breslein <https://github.com/tbreslein>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# ===============================================================================

cmake_minimum_required(VERSION 3.26)

project(
  tense
  LANGUAGES CXX
  DESCRIPTION "minimal library for matrix operations"
  VERSION 0.0.1)

set(TENSE_VERSION ${CMAKE_PROJECT_VERSION})

set(TENSE_TEST
    OFF
    CACHE BOOL "Builds the unit tests in ./test.")

set(TENSE_BENCH
    OFF
    CACHE BOOL "whether to build the bench binaries")

set(DEFAULT_COMPILE_DEFINITIONS SYSTEM_${SYSTEM_NAME_UPPER} INTERFACE
                                USE_INTERNAL_SIMD)

# ========================
# tense library & binaries
# ========================
set(TENSE_SOURCE_DIR "${tense_SOURCE_DIR}")
set(TENSE_BINARY_DIR "${tense_BINARY_DIR}")
set(TENSE_INCLUDE_DIR "${tense_SOURCE_DIR}")

include(GNUInstallDirs)

set(TENSE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(TENSE_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(TENSE_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_DATADIR}/cmake/tense")
set(TENSE_INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_DATADIR}/pkgconfig")

add_library(tense INTERFACE)
set_target_properties(tense PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(
  tense INTERFACE $<BUILD_INTERFACE:${TENSE_INCLUDE_DIR}>
                  $<INSTALL_INTERFACE:${TENSE_INSTALL_INCLUDE_DIR}>)

if(${TENSE_TEST} OR ${TENSE_BENCH})
  set(CMAKE_EXPORT_COMPILE_COMMANDS
      ON
      CACHE INTERNAL "Write compile_commands.json; needed for LSPs")

  if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
        ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  endif()

  # soft link for compile_commands
  file(CREATE_LINK ${CMAKE_BINARY_DIR}/compile_commands.json
       ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json SYMBOLIC)

  target_compile_definitions(tense INTERFACE cxx_std_20)
  find_package(OpenMP REQUIRED)
  add_library(tense_config INTERFACE)
  set(TENSE_DEPS OpenMP::OpenMP_CXX tense_config)
  if("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    target_compile_options(
      tense_config
      INTERFACE ${DEFAULT_COMPILE_OPTIONS}
                $<$<CONFIG:Release>:
                /Gw # whole program global optimization
                /GS- # buffer security check: no
                /GL # whole program optimization: enable link-time code
                    # generation
                # (disables Zi)
                /GF # enable string pooling
                >)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(
      tense_config
      INTERFACE ${DEFAULT_COMPILE_OPTIONS}
                -fdiagnostics-color=always
                -Wall
                -Wextra
                -Wextra-semi
                -Wcast-align
                -Wduplicated-branches
                -Wdeprecated-copy
                -Wdeprecated-declarations
                -Wduplicated-cond
                -Wformat=2
                -Wformat-truncation
                -Wignored-qualifiers
                -Wlogical-op
                -Wmissing-braces
                -Wmisleading-indentation
                -Wnon-virtual-dtor
                -Woverloaded-virtual
                -Wpedantic
                -Wreorder
                -Wreturn-type
                -Wsign-compare
                -Wsign-conversion
                -Wshadow
                -Wsuggest-final-methods
                -Wsuggest-final-types
                -Wsuggest-override
                -Wswitch-enum
                -Wuninitialized
                -Wunused
                -Wvolatile)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(
      tense_config
      INTERFACE ${DEFAULT_COMPILE_OPTIONS}
                -Wall
                -Wextra
                -Wformat=2
                -Wcast-align
                -Wcomma
                -Wdeprecated-declarations
                -Wignored-qualifiers
                -Wmissing-braces
                -Woverloaded-virtual
                -Wpedantic
                -Wreorder
                -Wreturn-type
                -Wsign-conversion
                -Wsuggest-override
                -Wswitch-enum
                -Wuninitialized
                -Wnon-virtual-dtor
                -fcolor-diagnostics)
  endif()

  include(cmake/CPM.cmake)
  if(${TENSE_TEST})
    cpmaddpackage("gh:doctest/doctest@2.4.11")
    add_subdirectory(test)
  endif()
  if(${TENSE_BENCH})
    cpmaddpackage("gh:martinus/nanobench#v4.3.11")
    add_subdirectory(bench)
  endif()
endif()

# ============
# installation
# ============

install(DIRECTORY "${TENSE_INCLUDE_DIR}/tense"
        DESTINATION "${TENSE_INSTALL_INCLUDE_DIR}")
install(TARGETS tense EXPORT tenseTargets)

export(
  TARGETS tense
  NAMESPACE tense::
  FILE "${TENSE_BINARY_DIR}/tenseTargets.cmake")
export(PACKAGE tense)
install(
  EXPORT tenseTargets
  NAMESPACE tense::
  DESTINATION "${TENSE_INSTALL_CMAKE_DIR}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
  "${TENSE_SOURCE_DIR}/data/tenseConfig.cmake.in"
  "${TENSE_BINARY_DIR}/tenseConfig.cmake"
  INSTALL_DESTINATION "${TENSE_INSTALL_CMAKE_DIR}"
  PATH_VARS TENSE_INSTALL_INCLUDE_DIR)
write_basic_package_version_file(
  "${TENSE_BINARY_DIR}/tenseConfigVersion.cmake"
  VERSION ${tense_VERSION}
  COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
install(FILES "${TENSE_BINARY_DIR}/tenseConfigVersion.cmake"
              "${TENSE_BINARY_DIR}/tenseConfig.cmake"
        DESTINATION "${TENSE_INSTALL_CMAKE_DIR}")

configure_file("${TENSE_SOURCE_DIR}/data/tense.pc.in"
               "${TENSE_BINARY_DIR}/tense.pc" @ONLY)
install(FILES "${TENSE_BINARY_DIR}/tense.pc"
        DESTINATION "${TENSE_INSTALL_PKGCONFIG_DIR}")
