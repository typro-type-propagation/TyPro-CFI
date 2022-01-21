#!/bin/bash

set -e

libevent_v=2.1.12

source _lib.sh

if [ "$2" = "lazy" ]; then
    if [ -f $INSTALL_DIR/lib/libevent.a ]; then
        echo "libevent already built!"
        exit 0
    fi
fi

$SCRIPT_DIR/lib_zlib.sh $MODE lazy

cd $SCRIPT_DIR
mkdir -p libevent
cd libevent

if [ ! -d libevent-${libevent_v}-$MODE ]; then
    if [ ! -f libevent-${libevent_v}-stable.tar.gz ]; then
        wget https://github.com/libevent/libevent/releases/download/release-${libevent_v}-stable/libevent-${libevent_v}-stable.tar.gz
    fi
    tar -xf libevent-${libevent_v}-stable.tar.gz
    mv libevent-${libevent_v}-stable libevent-${libevent_v}-$MODE
fi

cd ${SCRIPT_DIR}/libevent/libevent-${libevent_v}-$MODE

./configure --enable-static \
    --disable-thread-support \
    --disable-shared \
    --prefix=$INSTALL_DIR \
    --disable-openssl \
    --with-zlib=$INSTALL_DIR

rm -rf graph.*
nice -5 make -j$(nproc)
make install

echo "[DONE]"