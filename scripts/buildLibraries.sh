projectDir=$(PWD)
cd libraries/repositories/c++/boost
./b2 --no-cmake-config --prefix=$projectDir/libraries --build-dir=$projectDir/libraries --stagedir=$projectDir/libraries --with-filesystem install stage --link=shared
