projectDir=$(PWD)
cd libraries/repositories/c/FFmpeg
./configure --prefix=$projectDir/libraries --disable-programs --disable-ffplay --disable-ffprobe --disable-static --enable-shared
make
make install
