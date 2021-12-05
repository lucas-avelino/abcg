:: @echo off

set BUILD_TYPE=Release

:: echo "teste"
../emsdk/emsdk_env.bat

:: echo "teste"
:: Reset build directory
rd /s /q build
:: del /S /q build
mkdir build & cd build

:: Configure and build
emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .. & cmake --build . --config %BUILD_TYPE% -- -j %NUMBER_OF_PROCESSORS% & cd ..
