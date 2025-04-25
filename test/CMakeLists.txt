set(BINARY ${CMAKE_PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES true *.h *.cpp ../teacup/*Test.cpp ../interview/*.cpp) 
set(SOURCES ${TEST_SOURCES})

enable_testing()
# import google test
add_executable(${BINARY} ${TEST_SOURCES})
target_include_directories(${BINARY}  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../teacup ${CMAKE_CURRENT_SOURCE_DIR}/../interview)

add_test(NAME ${BINARY} COMMAND ${BINARY})
target_link_libraries(${BINARY} PUBLIC gtest gtest_main)
include(GoogleTest)
gtest_discover_tests(${BINARY})