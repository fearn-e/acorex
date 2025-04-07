cmake_minimum_required(VERSION 3.18)

# Project name
project(acorex)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Release)

include(FetchContent)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib
  GIT_PROGRESS TRUE
  GIT_TAG "5.5"
)

FetchContent_Declare(
  nfd
  GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended
  GIT_PROGRESS TRUE
  GIT_TAG "v1.2.1"
)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_PROGRESS TRUE
  GIT_TAG "v3.11.2"
)

FetchContent_Declare(
    flucoma-core
    GIT_REPOSITORY https://github.com/fearn-e/flucoma-core
    GIT_PROGRESS TRUE
    GIT_TAG "main"
)

#FetchContent_Declare(
#  clay
#  GIT_REPOSITORY https://github.com/nicbarker/clay
#  GIT_PROGRESS TRUE
#  GIT_TAG "v0.13"
#)
#

#FetchContent_MakeAvailable(raylib nfd clay json)
FetchContent_MakeAvailable(raylib nfd json flucoma-core)

# Add source and header files from src/ folder
# file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)
set(SOURCES
    src/main.cpp
    src/Explorer/RawView.cpp
    src/Explorer/RawView.h
    src/Utils/JSON.cpp
    src/Utils/JSON.h
    src/Utils/Data.h
    src/Utils/AudioFileLoader.cpp
    src/Utils/AudioFileLoader.h
    src/Utils/DatasetConversion.cpp
    src/Utils/DatasetConversion.h
)

# Add the executable
add_executable(acorex ${SOURCES})

# Include directories
target_include_directories(acorex PRIVATE
    ${PROJECT_SOURCE_DIR}/build/_deps/raylib-build/raylib/src
    ${PROJECT_SOURCE_DIR}/build/_deps/nfd-src/src/include
    ${PROJECT_SOURCE_DIR}/build/_deps/json-src/include
    ${PROJECT_SOURCE_DIR}/build/_deps/flucoma-core-src/include
)

# Link the libraries
target_link_libraries(acorex PRIVATE raylib nfd FLUID_DECOMPOSITION)