#=============================================================================
# SPDX-FileCopyrightText: 2023 Johnny Jazeix <jazeix@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#=============================================================================
cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0091 NEW)

project(Li_ri)

# get all the redist dll needed for windows when compiling with vc
set(CMAKE_INSTALL_UCRT_LIBRARIES 1)
include(InstallRequiredSystemLibraries)

### The current version number
set(LIRI_VERSION_MAJOR 3)
set(LIRI_VERSION_MINOR 0)
set(LIRI_VERSION_PATCH 0)
set(LIRI_VERSION ${LIRI_VERSION_MAJOR}.${LIRI_VERSION_MINOR})

set(LIRI_EXECUTABLE_NAME Li-ri)

option(USE_CONAN
  "Use conan to fetch dependencies" OFF)
if(USE_CONAN)
  find_package(SDL2 REQUIRED CONFIG)
  find_package(SDL2_mixer REQUIRED CONFIG)
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2 REQUIRED sdl2 SDL2_mixer)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${LIRI_EXECUTABLE_NAME} WIN32
  src/audio.cc
  src/audio.h
  src/ecran.cc
  src/ecran.h
  src/editeur.cc
  src/editeur.h
  src/jeux.cc
  src/jeux.h
  src/loco.cc
  src/loco.h
  src/main.cc
  src/menu.cc
  src/menu.h
  src/mouse.cc
  src/mouse.h
  src/sprite.cc
  src/sprite.h
  src/tableau.cc
  src/tableau.h
  src/utils.cc
  src/utils.h
  )

target_compile_definitions(${LIRI_EXECUTABLE_NAME} PUBLIC DATA_DIR="share/Li-ri")

if(USE_CONAN)
  target_link_libraries(${LIRI_EXECUTABLE_NAME} PRIVATE SDL2::SDL2 SDL2_mixer::SDL2_mixer)
else()
  target_include_directories(${LIRI_EXECUTABLE_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
  target_link_libraries(${LIRI_EXECUTABLE_NAME} ${SDL2_LINK_LIBRARIES})
endif()

include(GNUInstallDirs)
install(TARGETS ${LIRI_EXECUTABLE_NAME}
  ARCHIVE DESTINATION bin
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  BUNDLE DESTINATION .)

if(WIN32)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/ DESTINATION bin)
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/Sounds DESTINATION bin)
else()
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/ DESTINATION "${CMAKE_INSTALL_DATADIR}/Li-ri")
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/Sounds DESTINATION "${CMAKE_INSTALL_DATADIR}/Li-ri")
endif()
option(RILI_ENABLE_IPO
  "Enable interprocedural optimization (IPO/LTO) by the compiler" ON)
if(RILI_ENABLE_IPO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported)
  if(ipo_supported)
    message("interprocedural optimization is supported by the compiler")
    set_property(TARGET ${LIRI_EXECUTABLE_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING
      "interprocedural optimization is not supported by the compiler")
  endif()
endif()

include(CPackLists.txt)