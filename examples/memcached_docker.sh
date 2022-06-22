#!/bin/bash

set -e
set -o pipefail

memcached_v=1.6.10

source _lib.sh
$SCRIPT_DIR/lib_libevent.sh $MODE lazy
$SCRIPT_DIR/lib_zlib.sh $MODE lazy

cd $SCRIPT_DIR
mkdir -p memcached
cd memcached

if [ ! -d memcached-${memcached_v}-$MODE ]; then
    if [ ! -f memcached-${memcached_v}.tar.gz ]; then
        wget http://www.memcached.org/files/memcached-${memcached_v}.tar.gz
    fi
    tar -xf memcached-${memcached_v}.tar.gz
    mv memcached-${memcached_v} memcached-${memcached_v}-$MODE
fi

export LIBS="-lrt -lpthread"

cd ${SCRIPT_DIR}/memcached/memcached-${memcached_v}-$MODE

rm configure
cp ${SCRIPT_DIR}/memcached_configure ${SCRIPT_DIR}/memcached/memcached-${memcached_v}-$MODE/configure

#./configure --enable-static \
#     --disable-coverage \
#     --with-libevent=${SCRIPT_DIR}/memcached/libevent-${libevent_v}-stable/build

./configure --disable-coverage \
     --with-libevent=$INSTALL_DIR $CONFIGURE_FLAGS

rm -rf graph.*
nice -5 make -j$(nproc)

echo "[DONE MAKE]"

make test

echo "[DONE TESTS]"

echo "To re-run tests:"
echo "  cd $SCRIPT_DIR/memcached/memcached-${memcached_v}-$MODE"
echo "  make test"
