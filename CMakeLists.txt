cmake_minimum_required(VERSION 3.0)
project(MarbleMarcher)
set(CMAKE_CXX_STANDARD 17)

## DEPENDENCIES
set(SFML_DIR C:/Libraries/SFML-2.5.1/lib/cmake/SFML)
set(SFML_INCLUDE_DIR C:/Libraries/SFML-2.5.1/include)
set(SDK_DIRECTORY "C:/Program Files (x86)/Microsoft SDKs")
set(EIGEN3_INCLUDE_DIR C:/Libraries/eigen3/)
set(SFML_STATIC_LIBRARIES TRUE)
set(ANTTWEAKBAR_DIR C:/Libraries/AntTweakBar)

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

## TARGETS

add_subdirectory(src)
target_include_directories(MarbleMarcherSources PUBLIC
  src
  ${EIGEN3_INCLUDE_DIR}
  ${SFML_INCLUDE_DIR}
  ${ANTTWEAKBAR_DIR}/include
)

include_directories(${ANTTWEAKBAR_DIR}/include)
include_directories(${SDK_DIRECTORY}/Windows/v7.1/Include)

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
  ${ANTTWEAKBAR_DIR}/lib/AntTweakBar64.lib
  sfml-system
  sfml-window
  sfml-graphics
  sfml-audio
)
