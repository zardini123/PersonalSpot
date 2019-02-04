projectDir=$(PWD)
cd libraries/repositories/c++/boost
./b2 --no-cmake-config --prefix=$projectDir/libraries --build-dir=$projectDir/libraries --stagedir=$projectDir/libraries --with-filesystem install stage --link=shared

# gcc -shared -o libsqlite3.so -fPIC sqlite3.o -ldl -lpthread
