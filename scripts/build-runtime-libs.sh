#!/usr/bin/env bash

set -eu

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
ROOT=$(dirname "$SCRIPTPATH")
WORKDIR="$ROOT/sysroots/runtime_libs"
mkdir -p "$WORKDIR"
cd "$WORKDIR"

#CLANG_BIN="$ROOT/build/bin"
#if [ ! -d "$CLANG_BIN" ]; then
#	CLANG_BIN="$ROOT/llvm-typro/cmake-build-minsizerel/bin"
#fi
#if [ ! -d "$CLANG_BIN" ]; then
#	CLANG_BIN="/typro/build/bin"
#fi
CLANG="clang-10"
CLANGXX="clang++-10"



make_rt_libs () {
	if [ ! -d "$SYSROOT" ]; then
		echo "ERROR: $SYSROOT does not exist"
		return 1
	fi

	export CC="$CLANG $FLAGS --sysroot $SYSROOT"
	export CXX="$CLANGXX $FLAGS --sysroot $SYSROOT"
	#LD="$CLANG_BIN/ld.lld"
	export CFLAGS="$FLAGS --sysroot $SYSROOT -isystem $SYSROOT/usr/lib/gcc/$1/10/include -isystem $SYSROOT/usr/include/$1 -isystem $SYSROOT/usr/include"
	export CXXFLAGS="$FLAGS --sysroot $SYSROOT -isystem $SYSROOT/usr/include/c++/10 -isystem $SYSROOT/usr/include/$1/c++/10 -isystem $SYSROOT/usr/include/c++/10/backward -isystem $SYSROOT/usr/lib/gcc/$1/10/include -isystem $SYSROOT/usr/include/$1 -isystem $SYSROOT/usr/include"
	export LDFLAGS="$FLAGS $LINK_FLAGS -fuse-ld=lld --sysroot $SYSROOT -L$SYSROOT/usr/lib/$1 -L$SYSROOT/usr/lib/gcc/$1/10 -B$SYSROOT/usr/lib/$1 -B$SYSROOT/usr/lib/gcc/$1/10"

	rm -rf "$WORKDIR/$1"
	mkdir -p "$WORKDIR/$1"
	cd "$WORKDIR/$1"
	cmake "$ROOT/llvm-typro/lib/Typegraph" -DCMAKE_BUILD_TYPE=MinSizeRel -DTYPRO_LIBS_ONLY=1
	#cmake "$ROOT/llvm-typro/lib/Typegraph" -DCMAKE_BUILD_TYPE=Debug -DTYPRO_LIBS_ONLY=1

	make -j$(nproc) typro-instrumentation typro-rt
}



FLAGS="--target=x86_64-linux-gnu"
LINK_FLAGS=""
SYSROOT="$ROOT/sysroots/x86_64-linux-gnu"
make_rt_libs "x86_64-linux-gnu"

FLAGS="--target=aarch64-linux-gnu"
LINK_FLAGS=""
SYSROOT="$ROOT/sysroots/aarch64-linux-gnu"
make_rt_libs "aarch64-linux-gnu"

# mips64el,mips64el-linux-gnuabi64
FLAGS="--target=mips64el-linux-gnuabi64 -fPIC"
LINK_FLAGS="-Wl,-z,notext"
SYSROOT="$ROOT/sysroots/mips64el-linux-gnuabi64"
make_rt_libs "mips64el-linux-gnuabi64"
