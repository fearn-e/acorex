set -o nounset
set -o errexit

# Check Current Directory
    CURRENT_FOLDER=${PWD##*/}
    if [ "$CURRENT_FOLDER" != "acorex" ]; then
        echo "Incorrect working directory"
        exit
    fi
#


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
    if [ "$OSTYPE" == "darwin"* ]; then
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

echo "OS discovered as $currentOS"

# Download repos
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
        git -c advice.detachedHead=false clone --depth 1 -b main https://github.com/flucoma/flucoma-core
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
        cmake -DCMAKE_BUILD_TYPE=RELEASE -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .

        echo ""
        echo "creating release lib"
        echo ""

        cmake --build . --config Release
    fi

    cd ../..
#

# Install dependencies to deps folder
    echo "--------------------------------------------------"
    echo "installing dependencies and libraries"
    echo ""
    if [ -d "deps" ]; then
        rm -rfv deps
    fi
    mkdir -v deps

    if [ -d "libs" ]; then
        rm -rfv libs
    fi
    mkdir -v libs

    #copy dependency headerfiles
    cp -rv   deps-pre-build/eigen/Eigen/                                         deps/Eigen/
    cp -rv   deps-pre-build/eigen/unsupported/                                   deps/unsupported/
    cp -rv   deps-pre-build/flucoma-core/include/                                deps/flucoma-core/
    cp -rv   deps-pre-build/memory/include/foonathan/memory/                     deps/memory/
    cp -rv   deps-pre-build/hisstools_library/include/                           deps/hisstools_library/
    cp -rv   deps-pre-build/spectra/include/Spectra/                             deps/Spectra/
    cp -rv   deps-pre-build/json/include/nlohmann/                               deps/nlohmann/

    #copy foonathan_memory compiled lib files
    if [ "$currentOS" == "win" ]; then
        cp -v  deps-pre-build/compiled-memory/src/Debug/*                          libs/
        cp -v  deps-pre-build/compiled-memory/src/Release/*                        libs/
    elif [ "$currentOS" == "mac" ]; then
        cp -v  deps-pre-build/compiled-memory/src/foonathan_memory-*.a             libs/
    fi
    
    #copy extra compiled foonathan_memory headers
    cp -v      deps-pre-build/compiled-memory/src/config_impl.hpp                  deps/memory/
    cp -v      deps-pre-build/compiled-memory/src/container_node_sizes_impl.hpp    deps/memory/
#

echo "--------------------------------------------------"
echo "script finished"