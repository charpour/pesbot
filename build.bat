@echo off
pushd src
i686-w64-mingw32-g++ -g -Wall pesbot.cpp -o ../pesbot -lgdi32
popd