# Download repos
    echo "--------------------------------------------------"
    echo "downloading repos to deps-pre-build"
    echo ""

    if [ ! -d "deps-pre-build" ]
    then
        mkdir deps-pre-build
    fi

    cd deps-pre-build

    if [ ! -d "flucoma-core" ]
    then
        git clone https://github.com/flucoma/flucoma-core
    else
        echo "flucoma-core already exists, skipping download"
    fi
    echo ""

    if [ ! -d "eigen" ]
    then
        git clone https://gitlab.com/libeigen/eigen
    else
        echo "eigen already exists, skipping download"
    fi
    echo ""

    if [ ! -d "hisstools_library" ]
    then
        git clone --branch header-only https://github.com/alexharker/hisstools_library
    else
        echo "hisstools_library already exists, skipping download"
    fi
    echo ""

    if [ ! -d "memory" ]
    then
        git clone https://github.com/foonathan/memory
    else
        echo "memory already exists, skipping download"
    fi
    echo ""

#

# Build memory in a new directory
    echo "--------------------------------------------------"
    echo "cmaking foonathan memory libs"
    echo ""
    if [ -d "compiled-memory" ]
    then
        rm -r -f compiled-memory
    fi
    cp -r memory/ compiled-memory/

    #build foonathan_memory libs
    cd compiled-memory
    cmake -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .
    echo ""
    echo "creating debug lib"
    echo ""
    cmake --build . --config Debug
    echo ""
    echo "creating release lib"
    echo ""
    cmake --build . --config Release

    cd ../..
#

# Install dependencies to deps folder
    echo "--------------------------------------------------"
    echo "installing dependencies to deps folder"
    echo ""
    if [ -d "deps" ]
    then
        rm -r -f deps
    fi
    mkdir deps

    #copy dependency headerfiles
    cp -r   deps-pre-build/eigen/Eigen/                                       deps/Eigen/
    cp -r   deps-pre-build/flucoma-core/include/                              deps/flucoma-core/
    cp -r   deps-pre-build/memory/include/foonathan/memory                    deps/memory/
    cp -r   deps-pre-build/hisstools_library/include                          deps/hisstools_library/

    #copy foonathan_memory compiled lib files
    cp -r   deps-pre-build/compiled-memory/src/Debug/                             deps/memory/
    cp -r   deps-pre-build/compiled-memory/src/Release/                           deps/memory/

    #copy extra compiled foonathan_memory headers
    cp      deps-pre-build/compiled-memory/src/config_impl.hpp                    deps/memory/
    cp      deps-pre-build/compiled-memory/src/container_node_sizes_impl.hpp      deps/memory/
#

echo ""
echo "finished, acorex project is now ready to build"