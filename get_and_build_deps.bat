@echo off
rmdir /s /q temp
mkdir temp
cd temp

call git clone https://github.com/flucoma/flucoma-core
call git clone https://gitlab.com/libeigen/eigen
call git clone https://github.com/foonathan/memory
call git clone --branch header-only https://github.com/alexharker/hisstools_library

cd memory
cmake -DFOONATHAN_MEMORY_BUILD_TESTS=OFF -DFOONATHAN_MEMORY_BUILD_TOOLS=OFF -DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF .
cmake --build . --config Debug
cmake --build . --config Release

cd ..\..
xcopy /E temp\eigen\Eigen\ src\Eigen\
xcopy /E temp\flucoma-core\include\ src\flucoma-core\
xcopy /E temp\memory\include\foonathan\memory src\memory\
xcopy /E temp\hisstools_library\include src\hisstools_library\

xcopy temp\memory\src\Debug\ src\memory\
xcopy temp\memory\src\Release\ src\memory\

xcopy temp\memory\src\config_impl.hpp src\memory\
xcopy temp\memory\src\container_node_sizes_impl.hpp src\memory\

rmdir /s /q temp
exit /b
