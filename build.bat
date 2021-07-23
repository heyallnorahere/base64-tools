@echo off
rem i dont know batch too well, so im just assuming the preferred cmake command is "cmake."
cmake . -B build
cmake --build build --config Release