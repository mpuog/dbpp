cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    dbpp
    VERSION 0.1.0
    DESCRIPTION "C++ interface to DB"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    dbpp_dbpp
    source/dbpp.cpp
    source/odbc_db.cpp
    source/sqlite_db.cpp
    source/sqlite/sqlite3.c 
)
add_library(dbpp::dbpp ALIAS dbpp_dbpp)

include(GenerateExportHeader)
generate_export_header(
    dbpp_dbpp
    BASE_NAME dbpp
    EXPORT_FILE_NAME export/dbpp/dbpp_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(dbpp_dbpp PUBLIC DBPP_STATIC_DEFINE)
endif()

set_target_properties(
    dbpp_dbpp PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME dbpp
    OUTPUT_NAME dbpp
)

target_include_directories(
    dbpp_dbpp ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    dbpp_dbpp SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(dbpp_dbpp PUBLIC cxx_std_17)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${dbpp_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT dbpp_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of dbpp"
  )
endif()

include(cmake/dev-mode.cmake)
