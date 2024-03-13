currentOS="win"
# Get OS type
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # Mac OSX
        currentOS="mac"
    elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
        # Lightweight shell and GNU utilities compiled for Windows (part of MinGW), not sure if win32 can even happen?
        currentOS="win"
    else
        # Unknown, cygwin, linux-gnu
        echo "no official support for this OS, script functionality may not be as expected"
    fi
#

# Download repos
    echo "--------------------------------------------------"
    echo "downloading repos to deps-pre-build"
    echo ""

    if [ ! -d "deps-pre-build" ]; then
        mkdir deps-pre-build
    fi

    cd deps-pre-build

    if [ ! -d "flucoma-core" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b main https://github.com/flucoma/flucoma-core
    else
        echo "flucoma-core already exists, skipping download"
    fi
    echo ""

    if [ ! -d "eigen" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "3.4.0" https://gitlab.com/libeigen/eigen
    else
        echo "eigen already exists, skipping download"
    fi
    echo ""

    if [ ! -d "hisstools_library" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b header-only https://github.com/alexharker/hisstools_library
    else
        echo "hisstools_library already exists, skipping download"
    fi
    echo ""

    if [ ! -d "memory" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b main https://github.com/foonathan/memory
    else
        echo "memory already exists, skipping download"
    fi
    echo ""

    if [ ! -d "spectra" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "v1.0.1" https://github.com/yixuan/spectra
    else
        echo "spectra already exists, skipping download"
    fi
    echo ""

    if [ ! -d "json" ]; then
        git -c advice.detachedHead=false clone --depth 1 -b "v3.11.2" https://github.com/nlohmann/json
    else
        echo "json already exists, skipping download"
    fi
    echo ""
#

# Build memory in a new directory
    echo "--------------------------------------------------"
    echo "cmaking foonathan memory libs"
    echo ""
    if [ -d "compiled-memory" ]; then
        rm -r -f compiled-memory
    fi
    cp -r memory/ compiled-memory/

    #build foonathan_memory libs
    cd compiled-memory
    echo $currentOS

    if [ "$currentOS" == "win" ]; then
        cmake -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .

        echo ""
        echo "creating debug lib"
        echo ""

        cmake --build . --config Debug

        echo ""
        echo "creating release lib"
        echo ""

        cmake --build . --config Release
    elif [ "$currentOS" == "mac" ]; then
        cmake -DCMAKE_BUILD_TYPE=DEBUG -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .
        cmake -DCMAKE_BUILD_TYPE=RELEASE -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .
        
        echo ""
        echo "creating debug lib"
        echo ""

        cmake --build . --config Debug

        echo ""
        echo "creating release lib"
        echo ""

        cmake --build . --config Release
    fi

    cd ../..
#

# Install dependencies to deps folder
    echo "--------------------------------------------------"
    echo "installing dependencies to deps folder"
    echo ""
    if [ -d "deps" ]; then
        rm -r -f deps
    fi
    mkdir deps

    #copy dependency headerfiles
    cp -r   deps-pre-build/eigen/Eigen/                                         deps/Eigen/
    cp -r   deps-pre-build/eigen/unsupported/                                   deps/unsupported/
    cp -r   deps-pre-build/flucoma-core/include/                                deps/flucoma-core/
    cp -r   deps-pre-build/memory/include/foonathan/memory/                     deps/memory/
    cp -r   deps-pre-build/hisstools_library/include/                           deps/hisstools_library/
    cp -r   deps-pre-build/spectra/include/Spectra/                             deps/Spectra/
    cp -r   deps-pre-build/json/include/nlohmann/                               deps/nlohmann/

    #copy foonathan_memory compiled lib files
    cp -r   deps-pre-build/compiled-memory/src/Debug/                           deps/memory/
    cp -r   deps-pre-build/compiled-memory/src/Release/                         deps/memory/

    #copy extra compiled foonathan_memory headers
    cp      deps-pre-build/compiled-memory/src/config_impl.hpp                  deps/memory/
    cp      deps-pre-build/compiled-memory/src/container_node_sizes_impl.hpp    deps/memory/
#

echo ""
echo "script finished"