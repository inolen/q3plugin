Project(q3plugshim)

include (FindPkgConfig)

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    shim/[^.]*.cpp
    lib/msgpipe.*
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
