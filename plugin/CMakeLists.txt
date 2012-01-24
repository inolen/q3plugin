#/**********************************************************\
#
# Auto-generated CMakeLists.txt for the Q3Plugin project
#
#\**********************************************************/

# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

Project(${PLUGIN_NAME})

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    [^.]*.cpp
    [^.]*.h
    [^.]*.cmake
    ../lib/*.c
    ../lib/*.h
    )

include_directories(${PLUGIN_INCLUDE_DIRS})

# Generated files are stored in ${GENERATED} by the project configuration
SET_SOURCE_FILES_PROPERTIES(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

SOURCE_GROUP(Generated FILES
    ${GENERATED}
    )

SET( SOURCES
    ${GENERAL}
    ${GENERATED}
    )

# This will include Win/projectDef.cmake, X11/projectDef.cmake, Mac/projectDef
# depending on the platform
include_platform()

find_package (OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries (${PROJECT_NAME} ${OPENGL_LIBRARIES})

find_package (SDL REQUIRED)
#set(SDL_INCLUDE_DIR "/usr/local/include/SDL")
#set(SDL_LIBRARY "/usr/local/lib/libSDL.so")
include_directories(${SDL_INCLUDE_DIR})
target_link_libraries (${PROJECT_NAME} ${SDL_LIBRARY})