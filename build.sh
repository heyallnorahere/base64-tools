#!/bin/bash
CMAKE_EXE="$1"
if [[ -z "$CMAKE_EXE" ]]; then
    CMAKE_EXE="cmake"
fi
echo "CMake executable: $CMAKE_EXE"
echo "Configuring..."
mkdir -p build
"$CMAKE_EXE" "$PWD" -B "$PWD/build" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release | tee "build/cmake.log"
echo "Building..."
make -C build -j 8 | tee "build/make.log"
echo "Done building! Output written to $PWD/build"