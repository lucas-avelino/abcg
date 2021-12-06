:: @echo off

set BUILD_TYPE=Release

:: echo "teste"
../emsdk/emsdk_env.bat & rd /s /q build & mkdir build & cd build & emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% .. & cmake --build . --config %BUILD_TYPE% -- -j %NUMBER_OF_PROCESSORS% & cd ..
