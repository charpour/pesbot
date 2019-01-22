@echo off
pushd src
i686-w64-mingw32-g++ -o3 pesbot.cpp -o ../pesbot -lgdi32 -static-libgcc -static-libstdc++
popd