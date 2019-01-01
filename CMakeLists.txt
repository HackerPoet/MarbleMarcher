cmake_minimum_required(VERSION 3.0)
project(MarbleMarcher)
set(CMAKE_CXX_STANDARD 11)

find_package(Eigen3 3.3 REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_subdirectory(src)
target_include_directories(MarbleMarcherSources PUBLIC ${EIGEN3_INCLUDE_DIR})

add_executable(MarbleMarcher src/Main.cpp)
target_link_libraries(MarbleMarcher
  MarbleMarcherSources
  Eigen3::Eigen
  sfml-graphics
  sfml-audio
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
)

# Source: https://stackoverflow.com/questions/11813271/embed-resources-eg-shader-code-images-into-executable-library-with-cmake/27206982#27206982
# Creates C resources file from files in given directory
function(create_resources dir output)
  # Create empty output file
  file(WRITE ${output} "")
  # Collect input files
  file(GLOB bins ${dir}/*)
  # Iterate through input files
  foreach(bin ${bins})
    # Get short filename
    string(REGEX MATCH "([^/]+)$" filename ${bin})
    message("Generating binary resource for ${filename}")
    # Replace filename spaces & extension separator for C compatibility
    string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
    # Read hex data from file
    file(READ ${bin} filedata HEX)
    # Convert hex data for C compatibility
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
    # Append data to output file
    file(APPEND ${output} "const unsigned char ${filename}[] = {${filedata}};\nconst unsigned ${filename}_size = sizeof(${filename});\n")
  endforeach()
endfunction()

if(EXISTS "${PROJECT_SOURCE_DIR}/src/resources.c")
  message("Skipping resource assembly since src/resources.c exists. Remove the file to rebuild it.")
else()
  create_resources(assets src/resources.c)
endif()
