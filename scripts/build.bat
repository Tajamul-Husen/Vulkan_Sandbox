@echo off

set BUILD_DIR=build
set GENERATOR="MinGW Makefiles"
set PLATFORM=x64
set CONFIGURATION=Debug

mkdir %BUILD_DIR%
cd %BUILD_DIR%

cmake -G %GENERATOR% -DCMAKE_BUILD_TYPE=%CONFIGURATION% ..

cmake --build . --config %CONFIGURATION%

cd ..