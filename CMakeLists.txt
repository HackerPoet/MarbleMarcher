cmake_minimum_required(VERSION 2.6)
project(MarbleMarcher)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)

add_executable(MarbleMarcher src/Main.cpp)
target_link_libraries(MarbleMarcher MarbleMarcherSources)
