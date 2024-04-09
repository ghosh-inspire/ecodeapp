#
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: Copyright ECODE Contributors.
#
cmake_minimum_required(VERSION 3.22)

project(ecodeapp)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS TRUE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fno-pie -no-pie -Wextra -O2 -g")

add_definitions(-DNO_HARDWARE_BUILD)

#applies to all the targets in a particular CMakeLists.txt
include_directories(inc)
include_directories(munit)

add_executable(ecodecpu1 src/cpu1/ecodecpu.c)
add_executable(ecodecpu2 src/cpu2/ecodecpu.c)

target_link_options(ecodecpu1 PRIVATE -T${CMAKE_SOURCE_DIR}/src/cpu1/link.ld)
target_link_options(ecodecpu2 PRIVATE -T${CMAKE_SOURCE_DIR}/src/cpu2/link.ld)

add_library(ecodecb STATIC src/lib/ecodecb.c)
add_library(munit STATIC munit/munit.c)

target_link_libraries(ecodecpu1 PRIVATE ecodecb munit)
target_link_libraries(ecodecpu2 PRIVATE ecodecb munit)
