cmake_minimum_required(VERSION 3.0)
project(MarbleMarcher)
set(CMAKE_CXX_STANDARD 11)

## DEPENDENCIES

find_package(Eigen3 3.3 REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

## TARGETS

add_subdirectory(src)
target_include_directories(MarbleMarcherSources PUBLIC
  ${EIGEN3_INCLUDE_DIR}
)

add_executable(MarbleMarcher src/Main.cpp)
target_link_libraries(MarbleMarcher
  MarbleMarcherSources
  Eigen3::Eigen
  sfml-graphics
  sfml-audio
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_FILESYSTEM_LIBRARY}
)

## RESOURCES

# Source: https://stackoverflow.com/questions/11813271/embed-resources-eg-shader-code-images-into-executable-library-with-cmake/27206982#27206982
# Creates C resources file from files in given directory
function(create_resources dir output)
  file(WRITE ${output} "") # Create empty output file
  file(GLOB bins ${dir}/*) # Collect input files
  foreach(bin ${bins}) # Iterate through input files
    string(REGEX MATCH "([^/]+)$" filename ${bin}) # Get short filename
    message("Generating binary resource for ${filename}")
    # Replace filename spaces & extension separator for C compatibility
    string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
    file(READ ${bin} filedata HEX) # Read hex data from file
    # Convert hex data for C compatibility
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
    # Append data to output file
    file(APPEND ${output} "extern \"C\" const unsigned char ${filename}_bytes[] = {${filedata}};\nextern \"C\" const unsigned ${filename}_size = sizeof(${filename}_bytes);\n")
  endforeach()
endfunction()

function(hash_assets changed_assets)
  file(GLOB assets ${PROJECT_SOURCE_DIR}/assets/*) # Collect assets
  foreach(asset_file ${assets})
    string(REGEX MATCH "([^/]+)$" filename ${asset_file})
    set(hash_file "${PROJECT_SOURCE_DIR}/build/asset_hashes/${filename}.txt")
    file(MD5 ${asset_file} hash_value)
    if(EXISTS ${hash_file})
      file(READ ${hash_file} hash_from_file)
      if(NOT ${hash_from_file} STREQUAL ${hash_value})
        message("Changed ${filename}, rebuilding assets...")
        set(changed_assets true PARENT_SCOPE)
      endif()
    else()
      set(changed_assets true PARENT_SCOPE)
    endif()
    file(WRITE ${hash_file} ${hash_value})
  endforeach(asset_file assets)
endfunction(hash_assets)

# Only update the binary resources if they have been changed
# by checking whether the MD5 hash differs from the cached hash
set(resource_file src/resources.cpp)
set(changed_assets false)
hash_assets(changed_assets)
if(${changed_assets} OR NOT EXISTS ${resource_file})
  create_resources(assets ${resource_file})
else()
  message("Skipping resource assembly since assets folder has not been changed since last build.")
endif()
