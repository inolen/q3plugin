Project(q3pluginshim)

include (FindPkgConfig)

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    shim/[^.]*.cc
    MessagePipe.cc
    MessagePipe.h
    )

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET(SOURCES
    ${GENERAL}
    ${GENERATED}
    )

ADD_LIBRARY(${PROJECT_NAME} SHARED ${GENERAL})

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} dl)

# Output shim to main plugin directory.
set_target_properties ("${PROJECT_NAME}" PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
  LIBRARY_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
)
