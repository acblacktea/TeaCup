cmake_minimum_required(VERSION 3.10.0)
project(TEACUP VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(teacup)
include_directories(libs/googletest/gooletest/include)
# import google test
#add_subdirectory(teacup)
add_subdirectory(test)
add_subdirectory(libs/googletest)

MESSAGE("whole successfully")