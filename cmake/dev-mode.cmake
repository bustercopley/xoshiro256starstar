include(cmake/folders.cmake)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

include(cmake/lint-targets.cmake)

add_folders(Project)
