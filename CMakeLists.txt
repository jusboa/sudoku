include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()


enable_testing()

add_executable(
  unittest
  sudoku_test.cpp
)

target_include_directories(
    unittest
    PUBLIC
    ../app)

target_link_libraries(
  unittest
  app
  gtest_main
)

include(GoogleTest)
gtest_discover_tests(unittest)
