#!/bin/bash

set -e

pcre_v=8.45

source _lib.sh

if [ "$2" = "lazy" ]; then
    if [ -f $INSTALL_DIR/bin/pcre-config ]; then
        echo "PCRE already built!"
        exit 0
    fi
fi

cd $SCRIPT_DIR
mkdir -p lib_pcre
cd lib_pcre

if [ ! -d pcre-${pcre_v}-$MODE ]; then
    if [ ! -f pcre-${pcre_v}.tar.bz2 ]; then
        #wget https://ftp.pcre.org/pub/pcre/pcre-${pcre_v}.tar.gz
        wget https://sourceforge.net/projects/pcre/files/pcre/${pcre_v}/pcre-${pcre_v}.tar.bz2
    fi
    tar -xf pcre-${pcre_v}.tar.bz2
    mv pcre-${pcre_v} pcre-${pcre_v}-$MODE
fi
cd pcre-${pcre_v}-$MODE
./configure --enable-static --disable-shared --prefix="$INSTALL_DIR" --disable-cpp
make -j$(nproc)
make install
cd ..

echo "[DONE]"