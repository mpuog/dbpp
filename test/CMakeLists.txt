cmake_minimum_required(VERSION 3.14)

project(dbppTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/windows-set-path.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(dbpp REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(dbpp_test source/dbpp_test.cpp)
target_link_libraries(dbpp_test PRIVATE dbpp::dbpp)
target_compile_features(dbpp_test PRIVATE cxx_std_17)

add_test(NAME dbpp_test COMMAND dbpp_test)
windows_set_path(dbpp_test dbpp::dbpp)

# ---- End-of-file commands ----

add_folders(Test)
