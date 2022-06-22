#!/usr/bin/env bash

set -eu

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
ROOT=$(dirname "$SCRIPTPATH")
CLANG_BIN="$ROOT/build/bin"
if [ "$ROOT/llvm-typro/cmake-build-minsizerel/bin/clang" -nt "$CLANG_BIN/clang" ]; then
  CLANG_BIN=$ROOT/llvm-typro/cmake-build-minsizerel/bin
fi
echo "Clang directory: $CLANG_BIN"
[ -f $CLANG_BIN/llvm-ar ] || (echo "Please build targets: llvm-ar llvm-ranlib llvm-config" ; exit 1)
[ -f $CLANG_BIN/llvm-ranlib ] || (echo "Please build targets: llvm-ar llvm-ranlib llvm-config" ; exit 1)
[ -f $CLANG_BIN/llvm-config ] || (echo "Please build targets: llvm-ar llvm-ranlib llvm-config" ; exit 1)


# download source bundles
mkdir -p "$ROOT/sysroots"
cd "$ROOT/sysroots"
if [ ! -f 'musl-1.2.3.tar.gz' ]; then
  wget 'https://musl.libc.org/releases/musl-1.2.3.tar.gz'
fi


# === Build instruction ===

install_deb() {  # <url> <filename>
  mkdir -p "$SYSROOT"
  WORKDIR="$SYSROOT-work"
  mkdir -p "$WORKDIR"
  cd "$WORKDIR"
  if [ ! -f "$2" ]; then
    wget "$1"
  fi
  mkdir -p tmp
  cd tmp
  ar x "../$2"
  tar -xf data.tar.xz -C "$SYSROOT"
  cd ..
  rm -rf tmp
}

make_musl_libc() {
  mkdir -p "$SYSROOT"
  WORKDIR="$SYSROOT-work"
  mkdir -p "$WORKDIR"
  cd "$WORKDIR"
  tar -xf $ROOT/sysroots/musl-1.2.3.tar.gz
  cd musl-1.2.3

  # musl patches
  sed -i 's|#define VDSO_|// #define VDSO_|' arch/x86_64/syscall_arch.h
  sed -i 's|#define VDSO_|// #define VDSO_|' arch/aarch64/syscall_arch.h

  # patches to get openmp running
  #sed -i 's|void \*stdio_locks;$|void *stdio_locks;  char openmp_buffer[64];\n|' src/internal/pthread_impl.h
  #grep -q 'openmp_threadlocal_buffer' 'src/thread/pthread_self.c' || echo 'void *openmp_threadlocal_buffer(int offset) { return __pthread_self()->openmp_buffer + offset; }' >> src/thread/pthread_self.c

  optional=
  if test "$TARGET"; then
    optional="--target $TARGET"
  fi
  export CC=$CLANG_BIN/clang
  export AR=$CLANG_BIN/llvm-ar
  export RANLIB=$CLANG_BIN/llvm-ranlib
  export CFLAGS="$FLAGS -g"
  #export LDFLAGS="--sysroot $SYSROOT $LINK_FLAGS -Wl,--whole-archive $SYSROOT/usr/typro-lib/libtypro-rt.a -Wl,--no-whole-archive -lc++ -lc++abi -lunwind -fopenmp -lomp"
  export LDFLAGS="-g --sysroot $SYSROOT $LINK_FLAGS "$SYSROOT/usr/typro-lib/typro-rt-obj/*.o" -lc++ -lc++abi -lunwind -fopenmp -lomp -lgcc"
  export TG_PROTECTED_LIBC=1
  export TG_DYNLIB_SUPPORT=1
  export TG_LLVM_OUTPUT=/tmp/musl.ll
  export TG_GRAPH_OUTPUT=auto
  ./configure --prefix="$SYSROOT" --syslibdir="$SYSROOT/lib" $optional
  make -j$(nproc)
  make install

  sed -i 's|print-prog-name=ld|print-prog-name=lld|' $SYSROOT/bin/ld.musl-clang
  cp $SYSROOT/bin/musl-clang $SYSROOT/bin/musl-clang++
  cp $SYSROOT/bin/ld.musl-clang $SYSROOT/bin/ld.musl-clang++
  sed -i 's|clang|clang++|' $SYSROOT/bin/musl-clang++
  sed -i 's|clang|clang++|' $SYSROOT/bin/ld.musl-clang++
  sed -i 's|-nostdinc |-nostdinc -nostdinc++ |' $SYSROOT/bin/musl-clang++

  echo "Build musl libc in $SYSROOT"
}

make_clang_scripts() {
  # these scripts replace clang/lld, they modify some parameters before invoking the actual compiler

  #pushd .
  #cd $SYSROOT/bin
  #rm -f my-clang my-clang++ ld.my-clang
  #ln -s ../../../scripts/clang-wrapper.sh my-clang
  #ln -s ../../../scripts/clang-wrapper.sh my-clang++
  #ln -s ../../../scripts/ld-wrapper.sh ld.my-clang
  #popd

  cp "$ROOT/scripts/clang-wrapper.sh" "$SYSROOT/bin/my-clang"
  cp "$ROOT/scripts/clang-wrapper.sh" "$SYSROOT/bin/my-clang++"
  cp "$ROOT/scripts/ld-wrapper.sh" "$SYSROOT/bin/ld.my-clang"

  sed -i "s|CLANG_BIN_PATH|$CLANG_BIN|" "$SYSROOT/bin/my-clang"
  sed -i "s|CLANG_BIN_PATH|$CLANG_BIN|" "$SYSROOT/bin/my-clang++"
  sed -i "s|CLANG_BIN_PATH|$CLANG_BIN|" "$SYSROOT/bin/ld.my-clang"

  cp -rL /usr/lib/clang $SYSROOT/usr/lib/
}

download_dependencies() {
  install_deb "http://ftp.de.debian.org/debian/pool/main/g/gcc-10/libgcc-10-dev_10.2.1-6_$DEB_ARCH.deb" "libgcc-10-dev_10.2.1-6_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/l/linux/linux-libc-dev_5.10.106-1_$DEB_ARCH.deb" "linux-libc-dev_5.10.106-1_$DEB_ARCH.deb"
}

install_typro_libs() {
  rm -f "$SYSROOT/lib/libtypro-rt.so"
  rm -f "$SYSROOT/lib/libtypro-instrumentation.so"
  ln -s $(dirname "$CLANG_BIN")/lib/libtypro-rt.so "$SYSROOT/lib/libtypro-rt.so"
  ln -s $(dirname "$CLANG_BIN")/lib/libtypro-instrumentation.so "$SYSROOT/lib/libtypro-instrumentation.so"
}



# === Architecture-specific instructions ===

FLAGS=""
LINK_FLAGS=""
TARGET=""
SYSROOT="$ROOT/sysroots/x86_64-linux-musl"
DEB_ARCH=amd64
DEB_TARGET=x86_64-linux-gnu
download_dependencies
make_musl_libc
make_clang_scripts
install_typro_libs



FLAGS="--target=aarch64-linux-musl -fPIC"
LINK_FLAGS="-Wl,-z,notext"
TARGET="aarch64-linux-musl"
SYSROOT="$ROOT/sysroots/aarch64-linux-musl"
DEB_ARCH=arm64
DEB_TARGET=aarch64-linux-gnu
#download_dependencies
#make_musl_libc
#make_clang_scripts

