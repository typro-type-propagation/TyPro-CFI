#!/bin/bash

set -e

zlib_v=1.2.11

source _lib.sh

if [ "$2" = "lazy" ]; then
    if [ -f $INSTALL_DIR/lib/libz.a ]; then
        echo "zlib already built!"
        exit 0
    fi
fi

cd $SCRIPT_DIR
mkdir -p lib_zlib
cd lib_zlib

if [ ! -d zlib-${zlib_v}-$MODE ]; then
    if [ ! -f zlib-${zlib_v}.tar.gz ]; then
        wget https://zlib.net/fossils/zlib-${zlib_v}.tar.gz
    fi
    tar -xf zlib-${zlib_v}.tar.gz
    mv zlib-${zlib_v} zlib-${zlib_v}-$MODE
fi

cd zlib-${zlib_v}-$MODE

./configure --prefix="$INSTALL_DIR" --static
make -j$(nproc)
make install

echo "[DONE]"
