cmake_minimum_required(VERSION 2.8.9)

find_package(Qt5Quick 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)
if(MARSYAS_OPENGL)
  find_package(Qt5OpenGL 5.1 REQUIRED)
endif()

include_directories( ${CMAKE_SOURCE_DIR}/src/ )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyas/ )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyas/core )

add_subdirectory(common)

if(WITH_CPP11)
  add_subdirectory(inspector)
endif()

if(MARSYAS_AUDIOIO AND MARSYAS_REALTIME)
  add_subdirectory(MarPlayer)
  add_subdirectory(MarGrid2)
  add_subdirectory(MarPhasevocoder)
  add_subdirectory(MarLpc)
  if(MARSYAS_OPENGL)
    add_subdirectory(MarPanning)
    add_subdirectory(MarSndPeek)
    add_subdirectory(MarCorrelogram)
  endif()
endif()
