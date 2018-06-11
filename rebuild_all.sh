#
# Created by T. Nowak, p. szewd
#

rebuild()
{
    cd $1
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j$(nproc) # number of parallel make jobs = number of CPU cores
    cd ../..
}

rebuild common
rebuild sensor
rebuild central
rebuild config

mkdir ~/.RoSA
touch ~/.RoSA/logs.txt
touch ~/.RoSA/data.txt
