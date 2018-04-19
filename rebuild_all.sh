rebuild()
{
    cd $1
    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. # will be changed to Release
    make -j$(nproc) # number of paralell make jobs = number of CPU cores
    cd ../..
}

rebuild common
rebuild sensor
rebuild central
