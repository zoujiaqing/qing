
rm -rf CMakeFiles/ CMakeCache.txt
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
cmake -D CMAKE_BUILD_TYPE=DEBUG -D_CMAKE_TOOLCHAIN_PREFIX=llvm- ..
make

