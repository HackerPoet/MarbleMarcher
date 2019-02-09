cmake_minimum_required(VERSION 3.0)
project(MarbleMarcher)
set(CMAKE_CXX_STANDARD 11)

## DEPENDENCIES

find_package(Eigen3 3.3 REQUIRED)
find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

## TARGETS

set(MarbleMarcherDepTargets
  sfml-system
  sfml-window
  sfml-graphics
  sfml-audio
)

add_subdirectory(src)
target_include_directories(MarbleMarcherSources PUBLIC
  ${EIGEN3_INCLUDE_DIR}
)
target_compile_definitions(MarbleMarcherSources PRIVATE SFML_STATIC)

if(WIN32)
  add_executable(MarbleMarcher WIN32 src/Main.cpp src/Resource.rc assets/icon.ico)
  set_source_files_properties(icon.ico Resource.rc PROPERTIES LANGUAGE RC)
else()
  add_executable(MarbleMarcher src/Main.cpp)
endif()
target_compile_definitions(MarbleMarcher PRIVATE SFML_STATIC)
target_link_libraries(MarbleMarcher
  MarbleMarcherSources
)
