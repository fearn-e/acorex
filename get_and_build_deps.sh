rm -r -f temp
mkdir temp
cd temp

git clone https://github.com/flucoma/flucoma-core
git clone https://gitlab.com/libeigen/eigen
git clone https://github.com/foonathan/memory
git clone --branch header-only https://github.com/alexharker/hisstools_library

cd memory
cmake -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .
cmake --build . --config Debug
cmake --build . --config Release

cd ../..
cp --recursive temp/eigen/Eigen/ src/Eigen/
cp --recursive temp/flucoma-core/include/ src/flucoma-core/
cp --recursive temp/memory/include/foonathan/memory src/memory/
cp --recursive temp/hisstools_library/include src/hisstools_library/

cp --recursive temp/memory/src/Debug/ src/memory/
cp --recursive temp/memory/src/Release/ src/memory/

cp temp/memory/src/config_impl.hpp src/memory/
cp temp/memory/src/container_node_sizes_impl.hpp src/memory/