cmake_minimum_required(VERSION 3.0)
project(MarbleMarcher)
set(CMAKE_CXX_STANDARD 17)

#
# Try to find AntTweakBar library and include path.
# Once done this will define
#
# ANT_TWEAK_BAR_FOUND
# ANT_TWEAK_BAR_INCLUDE_DIR
# ANT_TWEAK_BAR_LIBRARY
#

IF (WIN32)
	IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	  SET( BITS "64" )
    ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      SET( BITS "" )
    ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )

	FIND_PATH( ANT_TWEAK_BAR_INCLUDE_DIR AntTweakBar.h
      PATHS
		${PROJECT_SOURCE_DIR}/../../external/AntTweakBar/include
		${PROJECT_SOURCE_DIR}/../external/AntTweakBar/include
		${PROJECT_SOURCE_DIR}/external/AntTweakBar/include
		$ENV{ANT_TWEAK_BAR_ROOT}/include
		DOC "The directory where AntTweakBar.h resides")

    FIND_LIBRARY( ANT_TWEAK_BAR_LIBRARY AntTweakBar${BITS}
        PATHS
		${PROJECT_SOURCE_DIR}/../../external/AntTweakBar/lib
		${PROJECT_SOURCE_DIR}/../external/AntTweakBar/lib
		${PROJECT_SOURCE_DIR}/external/AntTweakBar/lib
                $ENV{ANT_TWEAK_BAR_ROOT}/lib
                DOC "The AntTweakBar library")
ELSE (WIN32)

FIND_PATH(ANT_TWEAK_BAR_INCLUDE_DIR AntTweakBar.h
  PATHS
  ${LIBIGL_INCLUDE_DIR}/../external/AntTweakBar/include/
  ${PROJECT_SOURCE_DIR}/../../external/AntTweakBar/include/
  ${PROJECT_SOURCE_DIR}/../external/AntTweakBar/include/
  ${PROJECT_SOURCE_DIR}/external/AntTweakBar/include/
  /usr/local/include
  /usr/X11/include
  /usr/include
  NO_DEFAULT_PATH)

FIND_LIBRARY( ANT_TWEAK_BAR_LIBRARY AntTweakBar
  PATHS
		${LIBIGL_INCLUDE_DIR}/../external/AntTweakBar/lib
    ${PROJECT_SOURCE_DIR}/../../external/AntTweakBar/lib
    ${PROJECT_SOURCE_DIR}/../external/AntTweakBar/lib
		${PROJECT_SOURCE_DIR}/external/AntTweakBar/lib
    /usr/local
    /usr/X11
    /usr
  PATH_SUFFIXES
    a
    lib64
    lib
    dylib
    NO_DEFAULT_PATH
)

ENDIF (WIN32)


SET(ANTTWEAKBAR_FOUND "NO")
IF (ANT_TWEAK_BAR_INCLUDE_DIR AND ANT_TWEAK_BAR_LIBRARY)
	SET(ANTTWEAKBAR_FOUND "YES")
ENDIF (ANT_TWEAK_BAR_INCLUDE_DIR AND ANT_TWEAK_BAR_LIBRARY)

set(ANT_TWEAK_BAR_INCLUDE_DIR ${ANT_TWEAK_BAR_INCLUDE_DIR} ${ANT_TWEAK_BAR_INCLUDE_DIR}/../src/)

# message(FATAL_ERROR ${ANT_TWEAK_BAR_LIBRARY})

if(ANT_TWEAK_BAR_INCLUDE_DIR AND ANT_TWEAK_BAR_LIBRARY)
	message(STATUS "Found ANTTWEAKBAR: ${ANT_TWEAK_BAR_INCLUDE_DIR}")
else(ANT_TWEAK_BAR_INCLUDE_DIR AND ANT_TWEAK_BAR_LIBRARY)
	if (NOT ANTTWEAKBAR_FIND_QUIETLY)
		message(FATAL_ERROR "could NOT find ANTTWEAKBAR")
	endif (NOT ANTTWEAKBAR_FIND_QUIETLY)


endif(ANT_TWEAK_BAR_INCLUDE_DIR AND ANT_TWEAK_BAR_LIBRARY)

FILE(GLOB
  ANT_TWEAK_BAR_SOURCES
  ${ANT_TWEAK_BAR_INCLUDE_DIR}/../src/*.c
  ${ANT_TWEAK_BAR_INCLUDE_DIR}/../src/*.cpp)

## DEPENDENCIES

IF (WIN32)
set(SFML_DIR C:/Libraries/SFML-2.5.1_64/lib/cmake/SFML)
set(SFML_INCLUDE_DIR C:/Libraries/SFML-2.5.1_64/include)
set(SDK_DIRECTORY "C:/Program Files (x86)/Microsoft SDKs")
set(EIGEN3_INCLUDE_DIR C:/Libraries/eigen3/)
set(GLEW_INCLUDE_DIR C:/Libraries/GLEW)
set(GLM_INCLUDE_DIR C:/Libraries/glm)
set(SFML_STATIC_LIBRARIES TRUE)
ELSE (WIN32)
set(SFML_INCLUDE_DIR /usr/local/include)
set(EIGEN3_INCLUDE_DIR /usr/local/include/eigen3)
ENDIF (WIN32)

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

## TARGETS

add_subdirectory(src)
target_include_directories(MarbleMarcherSources PUBLIC
  src
  ${EIGEN3_INCLUDE_DIR}
  ${SFML_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}/include
  ${GLM_INCLUDE_DIR}
  ${ANT_TWEAK_BAR_INCLUDE_DIR}
)

include_directories(${ANT_TWEAK_BAR_LIBRARY})
IF (WIN32)
include_directories(${SDK_DIRECTORY}/Windows/v7.1/Include)
ENDIF (WIN32)

target_compile_definitions(MarbleMarcherSources PRIVATE SFML_STATIC)

if(WIN32)
  add_executable(MarbleMarcher WIN32 src/Main.cpp src/Resource.rc assets/icon.ico)
  set_source_files_properties(icon.ico Resource.rc PROPERTIES LANGUAGE RC)
else()
  add_executable(MarbleMarcher src/Main.cpp)
endif()

add_custom_command(TARGET MarbleMarcher PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/game_folder $<TARGET_FILE_DIR:MarbleMarcher>)


target_compile_definitions(MarbleMarcher PRIVATE SFML_STATIC)
target_link_libraries(MarbleMarcher
  MarbleMarcherSources
  ${ANT_TWEAK_BAR_LIBRARY}
  ${GLEW_INCLUDE_DIR}/lib/Release/x64/glew32.lib
  sfml-system
  sfml-window
  sfml-graphics
  sfml-audio
)
