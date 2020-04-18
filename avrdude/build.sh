#/bin/sh
# for static build
LDFLAGS=" -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread" CC=gcc CXX=g++ ./configure --enable-static=libusb_1_0 --enable-static=pthreads --enable-static=winpthreads --disable-udev --enable-static --disable-shared