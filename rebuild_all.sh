rebuild()
{
    cd $1
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. # will be changed to Release
    make -j$(nproc) # number of parallel make jobs = number of CPU cores
    cd ../..
}

rebuild common
rebuild sensor
rebuild central
