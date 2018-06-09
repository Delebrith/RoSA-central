build()
{
    cd $1
    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. # will be changed to Release
    make -j$(nproc) # number of parallel make jobs = number of CPU cores
    cd ../..
}

build common
build sensor
build central
build config

mkdir ~/.RoSA
touch ~/.RoSA/logs.txt