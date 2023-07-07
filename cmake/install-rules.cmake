if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/dbpp-${PROJECT_VERSION}"
      CACHE PATH ""
  )
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package dbpp)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT dbpp_Development
)

install(
    TARGETS dbpp_dbpp
    EXPORT dbppTargets
    RUNTIME #
    COMPONENT dbpp_Runtime
    LIBRARY #
    COMPONENT dbpp_Runtime
    NAMELINK_COMPONENT dbpp_Development
    ARCHIVE #
    COMPONENT dbpp_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    dbpp_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(dbpp_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${dbpp_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT dbpp_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${dbpp_INSTALL_CMAKEDIR}"
    COMPONENT dbpp_Development
)

install(
    EXPORT dbppTargets
    NAMESPACE dbpp::
    DESTINATION "${dbpp_INSTALL_CMAKEDIR}"
    COMPONENT dbpp_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
