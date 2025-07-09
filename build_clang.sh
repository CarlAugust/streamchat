rm -rf build
mkdir build && cd build

export CC=$(which clang)
export CXX=$(which clang++)

cmake .. -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXXcmake ..
make