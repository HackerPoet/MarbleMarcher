cmake_minimum_required(VERSION 3.0)
project(MarbleMarcher)
set(CMAKE_CXX_STANDARD 11)

find_package(Eigen3 3.3 REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_subdirectory(src)
target_include_directories(MarbleMarcherSources PUBLIC ${EIGEN3_INCLUDE_DIR})

add_executable(MarbleMarcher src/Main.cpp)
target_link_libraries(MarbleMarcher MarbleMarcherSources)
target_link_libraries(MarbleMarcher Eigen3::Eigen)
target_link_libraries(MarbleMarcher sfml-graphics sfml-audio)
