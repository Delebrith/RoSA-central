#
# Created by T. Nowak, p. szewd
#

build()
{
    cd $1
    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j$(nproc) # number of parallel make jobs = number of CPU cores
    cd ../..
}

build common
build sensor
build central
build config

mkdir ~/.RoSA
touch ~/.RoSA/logs.txt
touch ~/.RoSA/data.txt
