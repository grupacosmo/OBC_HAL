cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc.cmake \
    -DCMAKE_BUILD_TYPE=Debug
cmake --build build