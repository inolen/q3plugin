Project(q3plugshim)

include (FindPkgConfig)

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    shim/[^.]*.c
    shim/[^.]*.h
    msgpipe.*
    )

ADD_LIBRARY(${PROJECT_NAME} SHARED ${GENERAL})

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL_LIBRARY})

pkg_check_modules(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
link_directories(${GLIB2_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} ${GLIB2_LIBRARIES})

target_link_libraries(${PROJECT_NAME} dl)

# Output shim to main plugin directory.
set_target_properties ("${PROJECT_NAME}" PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
  LIBRARY_OUTPUT_DIRECTORY "${FB_BIN_DIR}/${PLUGIN_NAME}"
)