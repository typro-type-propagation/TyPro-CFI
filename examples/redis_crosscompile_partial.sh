#!/usr/bin/env bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
BASE_DIR="$( cd "$( dirname "$SCRIPT_DIR" )" &> /dev/null && pwd )"
cd $SCRIPT_DIR

# copy httpd to local
#
#
#


SYSROOT="$(dirname $SCRIPT_DIR)/sysroots/aarch64-linux-gnu"
TARGET="aarch64-linux-gnu"
GCC_VERSION="10"
export TARGET_FLAGS="--sysroot=$SYSROOT -m aarch64linux -mattr=+fp16fml+fullfp16+sha3+v8.5a+neon+fp-armv8+crypto"



cd redis/redis-arm64_enforcestatic/src
set -x

$BASE_DIR/examples/settings_enforcestatic.sh \
	"$BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/ld.lld" \
	$TARGET_FLAGS \
	-export-dynamic -EL --eh-frame-hdr -m aarch64linux -dynamic-linker /lib/ld-linux-aarch64.so.1 -o redis-server \
	$SYSROOT/usr/lib/$TARGET/crt1.o $SYSROOT/usr/lib/$TARGET/crti.o $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/crtbegin.o \
	-L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION -L$SYSROOT/usr/lib/$TARGET -L$SYSROOT/lib/$TARGET -L$SYSROOT/lib -L$SYSROOT/usr/lib \
	-L$BASE_DIR/examples/install_arm64_enforcestatic/lib -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/../../.. -L$BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/../lib \
	-plugin $BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/../lib/LLVMgold.so -plugin-opt=mcpu=generic -plugin-opt=O3 -plugin-opt=-debugger-tune=gdb -rpath $BASE_DIR/examples/install_enforcestatic/lib \
	adlist.o quicklist.o ae.o anet.o dict.o server.o sds.o zmalloc.o lzf_c.o lzf_d.o pqsort.o zipmap.o sha1.o ziplist.o release.o networking.o util.o object.o db.o replication.o rdb.o t_string.o t_list.o t_set.o t_zset.o t_hash.o config.o aof.o pubsub.o multi.o debug.o sort.o intset.o syncio.o cluster.o crc16.o endianconv.o slowlog.o scripting.o bio.o rio.o rand.o memtest.o crcspeed.o crc64.o bitops.o sentinel.o notify.o setproctitle.o blocked.o hyperloglog.o latency.o sparkline.o redis-check-rdb.o redis-check-aof.o geo.o lazyfree.o module.o evict.o expire.o geohash.o geohash_helper.o childinfo.o defrag.o siphash.o rax.o t_stream.o listpack.o localtime.o lolwut.o lolwut5.o lolwut6.o acl.o gopher.o tracking.o connection.o tls.o sha256.o timeout.o setcpuaffinity.o monotonic.o mt19937-64.o ../deps/hiredis/libhiredis.a ../deps/lua/src/liblua.a \
	-lm -ldl -lrt -lgcc --as-needed -lgcc_s --no-as-needed -lpthread -lc -lgcc --as-needed -lgcc_s --no-as-needed $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/crtend.o $SYSROOT/usr/lib/$TARGET/crtn.o

set +x
cd ../../..


cd redis/redis-arm64_ref/src
set -x

$BASE_DIR/examples/settings_ref.sh \
	"$BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/ld.lld" \
	$TARGET_FLAGS \
	-export-dynamic -EL --eh-frame-hdr -m aarch64linux -dynamic-linker /lib/ld-linux-aarch64.so.1 -o redis-server \
	$SYSROOT/usr/lib/$TARGET/crt1.o $SYSROOT/usr/lib/$TARGET/crti.o $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/crtbegin.o \
	-L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION -L$SYSROOT/usr/lib/$TARGET -L$SYSROOT/lib/$TARGET -L$SYSROOT/lib -L$SYSROOT/usr/lib \
	-L$BASE_DIR/examples/install_arm64_ref/lib -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/../../.. -L$BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/../lib \
	-plugin $BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/../lib/LLVMgold.so -plugin-opt=mcpu=generic -plugin-opt=O3 -plugin-opt=-debugger-tune=gdb -rpath $BASE_DIR/examples/install_ref/lib \
	adlist.o quicklist.o ae.o anet.o dict.o server.o sds.o zmalloc.o lzf_c.o lzf_d.o pqsort.o zipmap.o sha1.o ziplist.o release.o networking.o util.o object.o db.o replication.o rdb.o t_string.o t_list.o t_set.o t_zset.o t_hash.o config.o aof.o pubsub.o multi.o debug.o sort.o intset.o syncio.o cluster.o crc16.o endianconv.o slowlog.o scripting.o bio.o rio.o rand.o memtest.o crcspeed.o crc64.o bitops.o sentinel.o notify.o setproctitle.o blocked.o hyperloglog.o latency.o sparkline.o redis-check-rdb.o redis-check-aof.o geo.o lazyfree.o module.o evict.o expire.o geohash.o geohash_helper.o childinfo.o defrag.o siphash.o rax.o t_stream.o listpack.o localtime.o lolwut.o lolwut5.o lolwut6.o acl.o gopher.o tracking.o connection.o tls.o sha256.o timeout.o setcpuaffinity.o monotonic.o mt19937-64.o ../deps/hiredis/libhiredis.a ../deps/lua/src/liblua.a \
	-lm -ldl -lrt -lgcc --as-needed -lgcc_s --no-as-needed -lpthread -lc -lgcc --as-needed -lgcc_s --no-as-needed $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/crtend.o $SYSROOT/usr/lib/$TARGET/crtn.o


#

set +x
cd ../../..

# copy back
#
#
