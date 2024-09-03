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
    else
        # Unknown, cygwin, linux-gnu
        echo "no official support for this OS, script functionality may not be as expected, continue? (y/n)"
        CONFIRM="n"
        read -p "" CONFIRM
        if [ ! "$CONFIRM" == "y" ] && [ ! "$CONFIRM" == "Y" ]; then
            exit 
        fi
    fi
#

# Check Current Directory & OpenFrameworks Exists
    CURRENT_FOLDER=${PWD##*/}
    if [ "$CURRENT_FOLDER" != "acorex" ]; then
        echo "Incorrect working directory"
        exit
    fi

    OF_FOUND=false

    cd ..
    CURRENT_FOLDER=${PWD##*/}
    if [ "$CURRENT_FOLDER" == "myApps" ]; then
        cd ..
        CURRENT_FOLDER=${PWD##*/}
        if [ "$CURRENT_FOLDER" == "apps" ]; then
            cd myApps/acorex
            echo "openframeworks found"
            OF_FOUND=true
        else
            cd myApps/acorex
        fi
    else
        cd acorex
    fi

    if [ $OF_FOUND == false ]; then
        echo "openframeworks not found, downloading..."
        cd ..
        git -c advice.detachedHead=false clone --depth 1 -b "nightly" https://github.com/openframeworks/openframeworks
        
        cd openframeworks/apps
        
        if [ ! -d "myApps" ]; then
            mkdir myApps
        fi
        cd ../..

        cp -rv acorex/ openframeworks/apps/myApps/acorex/
        rm -rfv acorex

        cd openframeworks/scripts

        if [ "$currentOS" == "win" ]; then
            cd vs
            source download_libs.sh
        elif [ "$currentOS" == "mac" ]; then
            cd osx
            source download_libs.sh
        fi

        cd ../../apps/myApps/acorex
    fi
#

echo "OS discovered as $currentOS"

# Download repos & OF addons
    echo "--------------------------------------------------"
    echo "downloading openframeworks addons"
    echo ""

    cd ../../../addons

    if [ ! -d "ofxDropdown" ]; then
        git clone --depth 1 -b master https://github.com/fearn-e/ofxDropdown
        echo ""
    fi

    if [ ! -d "ofxAudioFile" ]; then
        git clone --depth 1 -b master https://github.com/fearn-e/ofxAudioFile
        echo ""
    fi

    cd ../apps/myApps/acorex

    echo "--------------------------------------------------"
    echo "downloading repos to deps-pre-build"
    echo ""

    declare -i FORCE_DOWNLOAD_TIP=0

    if [ $FORCE_DOWNLOAD == true ] && [ -d "deps-pre-build" ]; then
        rm -rfv deps-pre-build
    fi

    if [ ! -d "deps-pre-build" ]; then
        mkdir -v deps-pre-build
    fi

    cd deps-pre-build

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
        echo "$FORCE_DOWNLOAD_TIP downloads already exist and were skipped (force this step with -d)"
        echo ""
    fi
#

# Build memory in a new directory
    echo "--------------------------------------------------"
    echo "cmaking foonathan memory libs"
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
        echo "creating debug lib"
        echo ""

        cmake --build . --config Debug

        echo ""
        echo "creating release lib"
        echo ""

        cmake --build . --config Release
    elif [ $MEMORY_CMAKE_NEEDED == true ] && [ "$currentOS" == "mac" ]; then
        echo "macos"
        cmake "-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64" -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .

        echo ""
        echo "creating release lib"
        echo ""

        cmake --build .
    fi

    cd ../..
#

# Install dependencies to deps folder
    echo "--------------------------------------------------"
    echo "installing dependencies and libraries"
    echo ""
    if [ -d "deps" ]; then
        rm -rf deps
    fi
    mkdir -v deps

    if [ -d "libs" ]; then
        rm -rf libs
    fi
    mkdir -v libs

    echo "copying headers..."
    #copy dependency headerfiles
    cp -r   deps-pre-build/eigen/Eigen/                                         deps/Eigen/
    cp -r   deps-pre-build/eigen/unsupported/                                   deps/unsupported/
    cp -r   deps-pre-build/flucoma-core/include/                                deps/flucoma-core/
    cp -r   deps-pre-build/memory/include/foonathan/memory/                     deps/memory/
    cp -r   deps-pre-build/hisstools_library/include/                           deps/hisstools_library/
    cp -r   deps-pre-build/spectra/include/Spectra/                             deps/Spectra/
    cp -r   deps-pre-build/json/include/nlohmann/                               deps/nlohmann/

    echo "copying libs..."
    #copy foonathan_memory compiled lib files
    if [ "$currentOS" == "win" ]; then
        cp  deps-pre-build/compiled-memory/src/Debug/*                          libs/
        cp  deps-pre-build/compiled-memory/src/Release/*                        libs/
    elif [ "$currentOS" == "mac" ]; then
        cp  deps-pre-build/compiled-memory/src/libfoonathan_memory-*.a             libs/
    fi
    
    #copy extra compiled foonathan_memory headers
    cp      deps-pre-build/compiled-memory/src/config_impl.hpp                  deps/memory/
    cp      deps-pre-build/compiled-memory/src/container_node_sizes_impl.hpp    deps/memory/
#

echo "--------------------------------------------------"
echo "script finished"