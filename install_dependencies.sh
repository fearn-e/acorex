# The MIT License (MIT)
# 
# Copyright (c) 2024 Elowyn Fearne
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
# to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


set -o nounset
set -o errexit

# Optional args
    FORCE_DOWNLOAD=false
    FORCE_COMPILE=false

    ARG1=${1:-"n"}
    ARG2=${2:-"n"}

    if [ $ARG1 == "-d" ] || [ $ARG2 == "-d" ]; then
        FORCE_DOWNLOAD=true
    elif [ $ARG1 == "-c" ] || [ $ARG2 == "-c" ]; then
        FORCE_COMPILE=true
    elif [ $ARG1 == "-h" ] || [ $ARG2 == "-h" ]; then
        echo "Usage: ./build-deps.sh [-d/c/h] [-d/c/h]"
        echo "  -d  force (re)download of all dependencies"
        echo "  -c  force (re)compile of libraries"
        echo "  -h  display this help message"
        exit
    fi
#

# Get OS type
    currentOS="win"
    if echo "$OSTYPE" | grep -q "darwin.*"; then
        # Mac OSX
        currentOS="mac"
    elif [ "$OSTYPE" == "msys" ] || [ "$OSTYPE" == "win32" ]; then
        # Lightweight shell and GNU utilities compiled for Windows (part of MinGW), not sure if win32 can even happen?
        currentOS="win"
    elif [ "$OSTYPE" == "linux-gnu" ]; then
        # Linux
        currentOS="linux"
    else
        # Unknown, cygwin
        echo "no official support for this OS, script functionality may not be as expected, continue? (y/n)"
        CONFIRM="n"
        read -p "" CONFIRM
        if [ ! "$CONFIRM" == "y" ] && [ ! "$CONFIRM" == "Y" ]; then
            exit 
        fi
    fi
#

echo "OS discovered as $currentOS"
echo ""

# Check Current Directory
    CURRENT_FOLDER=${PWD##*/}
    if [ "$CURRENT_FOLDER" != "acorex" ]; then
        echo "Incorrect working directory"
        exit
    fi
#

# Download dependencies
    echo "--------------------------------------------------"
    echo "downloading dependencies to tmp"
    echo ""

    declare -i FORCE_DOWNLOAD_TIP=0

    if [ $FORCE_DOWNLOAD == true ] && [ -d "tmp" ]; then
        rm -rfv tmp
    fi

    if [ ! -d "tmp" ]; then
        mkdir -v tmp
    fi

    cd tmp

    if [ ! -d "raylib" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "5.5" https://github.com/raysan5/raylib raylib

        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ ! -d "nfd-extended" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "v1.2.1" https://github.com/btzy/nativefiledialog-extended nfd-extended

        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ ! -d "clay" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "v0.13" https://github.com/nicbarker/clay
        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ ! -d "flucoma-core" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b acorex https://github.com/fearn-e/flucoma-core
        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ ! -d "eigen" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "3.4.0" https://gitlab.com/libeigen/eigen
        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ ! -d "hisstools_library" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b header-only https://github.com/alexharker/hisstools_library
        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ ! -d "memory" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b main https://github.com/foonathan/memory
        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ ! -d "spectra" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "v1.0.1" https://github.com/yixuan/spectra
        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ ! -d "json" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "v3.11.2" https://github.com/nlohmann/json
        echo ""
    else
        FORCE_DOWNLOAD_TIP+=1
    fi

    if [ $FORCE_DOWNLOAD_TIP > 0 ]; then
        echo "$FORCE_DOWNLOAD_TIP downloads in tmp already exist and were skipped (force this step with -d)"
        echo ""
    fi

    cd ..
#

# Build memory in a new directory
    cd tmp
    echo "--------------------------------------------------"
    echo "building foonathan memory libs"
    echo ""
    MEMORY_CMAKE_NEEDED=false

    if [ $FORCE_COMPILE == true ] && [ -d "compiled-memory" ]; then
        rm -rfv compiled-memory
        cp -rv memory/ compiled-memory/
        MEMORY_CMAKE_NEEDED=true
    fi

    if [ ! -d "compiled-memory" ]; then
        cp -rv memory/ compiled-memory/
        MEMORY_CMAKE_NEEDED=true
    fi

    if [ $MEMORY_CMAKE_NEEDED == false ]; then
        echo "compiled-memory already exists, skipping compilation (force this step with -c)"
        echo ""
    fi

    #build foonathan_memory libs
    cd compiled-memory

    if [ $MEMORY_CMAKE_NEEDED == true ] && [ "$currentOS" == "win" ]; then
        echo "windows"
        cmake -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .

        echo ""
        echo "building foonathan_memory lib"
        echo ""

        cmake --build . --config Release

    elif [ $MEMORY_CMAKE_NEEDED == true ] && [ "$currentOS" == "mac" ]; then
        echo "macos"
        cmake "-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64" -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .

        echo ""
        echo "building foonathan_memory lib"
        echo ""

        cmake --build .

    elif [ $MEMORY_CMAKE_NEEDED == true ] && [ "$currentOS" == "linux" ]; then
        echo "linux"
        cmake -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .

        echo ""
        echo "building foonathan_memory lib"
        echo ""

        cmake --build .
    fi

    cd ../..
#

# Build nfd-extended in a new directory
    cd tmp
    echo "--------------------------------------------------"
    echo "building nfd-extended libs"
    echo ""
    NFD_CMAKE_NEEDED=false

    if [ $FORCE_COMPILE == true ] && [ -d "compiled-nfd-extended" ]; then
        rm -rfv compiled-nfd-extended
        cp -rv nfd-extended/ compiled-nfd-extended/
        NFD_CMAKE_NEEDED=true
    fi

    if [ ! -d "compiled-nfd-extended" ]; then
        cp -rv nfd-extended/ compiled-nfd-extended/
        NFD_CMAKE_NEEDED=true
    fi

    if [ $NFD_CMAKE_NEEDED == false ]; then
        echo "compiled-nfd-extended already exists, skipping compilation (force this step with -c)"
        echo ""
    fi

    #build nfd-extended libs
    cd compiled-nfd-extended

    if [ $NFD_CMAKE_NEEDED == true ] && [ "$currentOS" == "win" ]; then
        echo "windows"
        mkdir build
        cd build
        cmake -DNFD_BUILD_TESTS=OFF -DNFD_INSTALL=OFF ..

        echo ""
        echo "building nfd-extended lib"
        echo ""

        cmake --build . --config Release
        cd ..

    elif [ $NFD_CMAKE_NEEDED == true ] && [ "$currentOS" == "mac" ]; then
        echo "macos"
        mkdir build
        cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DNFD_BUILD_TESTS=OFF -DNFD_INSTALL=OFF ..

        echo ""
        echo "building nfd-extended lib"
        echo ""

        cmake --build .
        cd ..

    elif [ $NFD_CMAKE_NEEDED == true ] && [ "$currentOS" == "linux" ]; then
        echo "linux"
        mkdir build
        cd build
        cmake -DCMAKE_BUILD_TYPE=Release -DNFD_BUILD_TESTS=OFF -DNFD_INSTALL=OFF ..

        echo ""
        echo "building nfd-extended lib"
        echo ""

        cmake --build .
        cd ..
    fi

    cd ../..
#

# ADD RAYLIB BUILD STEP

# Install dependencies to include and libs folders
    echo "--------------------------------------------------"
    echo "installing headers and libraries"
    echo ""
    if [ -d "include" ]; then
        rm -rf include
    fi
    mkdir -v include

    if [ -d "libs" ]; then
        rm -rf libs
    fi
    mkdir -v libs

    echo "copying headers..."
    #copy dependency headerfiles
    cp -r   tmp/eigen/Eigen/                                         include/Eigen/
    cp -r   tmp/eigen/unsupported/                                   include/unsupported/
    cp -r   tmp/flucoma-core/include/                                include/flucoma-core/
    cp -r   tmp/memory/include/foonathan/memory/                     include/memory/
    cp -r   tmp/hisstools_library/include/                           include/hisstools_library/
    cp -r   tmp/spectra/include/Spectra/                             include/Spectra/
    cp -r   tmp/json/include/nlohmann/                               include/nlohmann/
    mkdir   include/clay
    cp      tmp/clay/clay.h                                          include/clay/clay.h
    mkdir   include/raylib
    cp      tmp/raylib/src/raylib.h                                  include/raylib/raylib.h

    echo "copying libs..."
    #copy compiled lib files
    if [ "$currentOS" == "win" ]; then
        cp  tmp/compiled-memory/src/Release/*.a                      libs/
        cp  tmp/compiled-nfd-extended/build/src/*.a                  libs/
    elif [ "$currentOS" == "mac" ] || [ "$currentOS" == "linux" ]; then
            cp  tmp/compiled-memory/src/libfoonathan_memory-*.a      libs/
            cp  tmp/compiled-nfd-extended/build/src/libnfd.a         libs/
    # ADD RAYLIB LIBS
    fi

    #copy extra compiled files
    cp      tmp/compiled-memory/src/config_impl.hpp                  include/memory/
    cp      tmp/compiled-memory/src/container_node_sizes_impl.hpp    include/memory/
    cp -r   tmp/compiled-nfd-extended/src/include/                   include/nfd-extended/
#

echo "--------------------------------------------------"
echo "script finished"
exit
