#!/usr/bin/env bash

set -eu

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
ROOT=$(dirname "$SCRIPTPATH")
CLANG_BIN="$ROOT/build/bin"
if [ "$ROOT/llvm-typro/cmake-build-minsizerel/bin/clang" -nt "$CLANG_BIN/clang" ]; then
  CLANG_BIN=$ROOT/llvm-typro/cmake-build-minsizerel/bin
fi

CLANG=/usr/bin/clang-10
CLANGPP=/usr/bin/clang++-10
CLANG_AR=/usr/bin/llvm-ar-10
CLANG_RANLIB=/usr/bin/llvm-ranlib-10
CLANG_CONFIG=/usr/bin/llvm-config-10

if [ ! -f $CLANG_AR ]; then
  CLANG_AR="$CLANG_BIN/llvm-ar"
  CLANG_RANLIB="$CLANG_BIN/llvm-ranlib"
  CLANG_CONFIG="$CLANG_BIN/llvm-config"
fi

[ -f $CLANG_AR ] || (echo "Please build targets: llvm-ar llvm-ranlib llvm-config" ; exit 1)
[ -f $CLANG_RANLIB ] || (echo "Please build targets: llvm-ar llvm-ranlib llvm-config" ; exit 1)
[ -f $CLANG_CONFIG ] || (echo "Please build targets: llvm-ar llvm-ranlib llvm-config" ; exit 1)


# download source bundles
cd $ROOT/sysroots
if [ ! -f 'musl-1.2.3.tar.gz' ]; then
  wget 'https://musl.libc.org/releases/musl-1.2.3.tar.gz'
fi
if [ ! -f 'libcxxabi-10.0.1.src.tar.xz' ]; then
  wget 'https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.1/libcxxabi-10.0.1.src.tar.xz'
fi
if [ ! -f 'libunwind-10.0.1.src.tar.xz' ]; then
  wget 'https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.1/libunwind-10.0.1.src.tar.xz'
fi
if [ ! -f 'libcxx-10.0.1.src.tar.xz' ]; then
  wget 'https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.1/libcxx-10.0.1.src.tar.xz'
fi
if [ ! -f 'openmp-10.0.1.src.tar.xz' ]; then
  wget 'https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.1/openmp-10.0.1.src.tar.xz'
fi
if [ ! -f 'compiler-rt-10.0.1.src.tar.xz' ]; then
  wget 'https://github.com/llvm/llvm-project/releases/download/llvmorg-10.0.1/compiler-rt-10.0.1.src.tar.xz'
fi


# === Build instruction ===

DEFINITIONS="-Dqsort=_internal_qsort -Dqsort_r=_internal_qsort_r -D__qsort=_internal___qsort -D__qsort_r=_internal___qsort_r \
-D__get_handler_set=_internal___get_handler_set -D__libc_sigaction=_internal___libc_sigaction -D__sigaction=_internal___sigaction -Dsigaction=_internal_sigaction \
-Dbsd_signal=_internal_bsd_signal -D__sysv_signal=_internal___sysv_signal -Dsignal=_internal_signal \
-D__pthread_create=_internal___pthread_create -Dpthread_create=_internal_pthread_create -D__pthread_exit=_internal___pthread_exit -D__pthread_exit=_internal___pthread_exit \
-D__do_cleanup_push=_internal___do_cleanup_push -D__do_cleanup_pop=_internal___do_cleanup_pop -D__tl_lock=_internal___tl_lock -D__tl_unlock=_internal___tl_unlock -D__tl_sync=_internal___tl_sync \
-D__pthread_key_create=_internal___pthread_key_create -D__pthread_key_delete=_internal___pthread_key_delete -D__pthread_tsd_run_dtors=_internal___pthread_tsd_run_dtors -D__pthread_tsd_size=_internal___pthread_tsd_size -D__pthread_tsd_main=_internal___pthread_tsd_main \
-Dpthread_atfork=_internal_pthread_atfork -D__fork_handler=_internal___fork_handler"

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

  # renaming functions
  sed -i 's|__qsort|_internal___qsort|' src/stdlib/qsort.c src/stdlib/qsort_nr.c
  sed -i 's|__qsort_r|_internal___qsort_r|' src/stdlib/qsort.c src/stdlib/qsort_nr.c
  sed -i 's|__get_handler_set|_internal___get_handler_set|' src/signal/sigaction.c
  sed -i 's|__libc_sigaction|_internal___libc_sigaction|' src/signal/sigaction.c
  sed -i 's|__sigaction|_internal___sigaction|' src/signal/sigaction.c
  sed -i 's|bsd_signal|_internal_bsd_signal|' src/signal/signal.c
  sed -i 's|__sysv_signal|_internal___sysv_signal|' src/signal/signal.c
  sed -i 's|[(]signal|(_internal_signal|' src/signal/signal.c
  sed -i 's|__pthread_create|_internal___pthread_create|' src/stdlib/qsort.c src/thread/pthread_create.c
  sed -i 's|__pthread_exit|_internal___pthread_exit|' src/stdlib/qsort.c src/thread/pthread_create.c
  sed -i 's|__pthread_key_create|_internal___pthread_key_create|' src/stdlib/qsort.c src/thread/pthread_key_create.c
  sed -i 's|__pthread_key_delete|_internal___pthread_key_delete|' src/stdlib/qsort.c src/thread/pthread_key_create.c
  sed -i 's|__pthread_tsd_run_dtors|_internal___pthread_tsd_run_dtors|' src/stdlib/qsort.c src/thread/pthread_key_create.c

  # patches to get openmp running
  #sed -i 's|void \*stdio_locks;$|void *stdio_locks;  char openmp_buffer[64];\n|' src/internal/pthread_impl.h
  #grep -q 'openmp_threadlocal_buffer' 'src/thread/pthread_self.c' || echo 'void *openmp_threadlocal_buffer(int offset) { return __pthread_self()->openmp_buffer + offset; }' >> src/thread/pthread_self.c

  optional=
  if test "$TARGET"; then
    optional="--target $TARGET"
  fi
  export CC=$CLANG
  export AR=$CLANG_AR
  export RANLIB=$CLANG_RANLIB
  export CFLAGS="$FLAGS $DEFINITIONS"
  export LDFLAGS="--sysroot $SYSROOT $LINK_FLAGS"
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

  sed -i "s|CLANG_BIN_PATH/clang|$CLANG|" "$SYSROOT/bin/my-clang"
  sed -i "s|CLANG_BIN_PATH/clang++|$CLANGPP|" "$SYSROOT/bin/my-clang++"
  sed -i "s|CLANG_BIN_PATH/clang|$CLANG|" "$SYSROOT/bin/my-clang++"
  sed -i "s|CLANG_BIN_PATH/clang|$CLANG|" "$SYSROOT/bin/ld.my-clang"
}

download_dependencies() {
  install_deb "http://ftp.de.debian.org/debian/pool/main/g/gcc-10/libgcc-10-dev_10.2.1-6_$DEB_ARCH.deb" "libgcc-10-dev_10.2.1-6_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/l/linux/linux-libc-dev_5.10.106-1_$DEB_ARCH.deb" "linux-libc-dev_5.10.106-1_$DEB_ARCH.deb"
}

make_libcxx() {
  mkdir -p "$SYSROOT"
  WORKDIR="$SYSROOT-work"
  mkdir -p "$WORKDIR"
  cd "$WORKDIR"
  tar -xf $ROOT/sysroots/libcxxabi-10.0.1.src.tar.xz
  tar -xf $ROOT/sysroots/libcxx-10.0.1.src.tar.xz
  tar -xf $ROOT/sysroots/libunwind-10.0.1.src.tar.xz

  export CC=$SYSROOT/bin/my-clang
  export CXX=$SYSROOT/bin/my-clang
  export AR=$CLANG_AR
  export RANLIB=$CLANG_RANLIB
  export CFLAGS="$FLAGS $DEFINITIONS"
  export CXXFLAGS="$FLAGS $DEFINITIONS"
  export LDFLAGS="--sysroot $SYSROOT $LINK_FLAGS"

  # libunwind
  rm -rf libunwind-build
  mkdir -p libunwind-build
  cd libunwind-build

  cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=$SYSROOT -DCMAKE_CXX_COMPILER=$CXX \
    -DCMAKE_CXX_COMPILER_WORKS=1 \
    -DLIBCXX_ENABLE_RTTI=On -DLIBCXX_ENABLE_EXCEPTIONS=On -DLIBCXXABI_ENABLE_EXCEPTIONS=On \
    -DLIBCXX_HERMETIC_STATIC_LIBRARY=On -D=LIBCXXABI_HERMETIC_STATIC_LIBRARY=On -DLIBUNWIND_HERMETIC_STATIC_LIBRARY=On \
    ../libunwind-10.0.1.src
  # LIBUNWIND_ENABLE_CROSS_UNWINDING LIBUNWIND_TARGET_TRIPLE LIBUNWIND_SYSROOT
  make -j$(nproc)
  make install
  cd ..
  echo "[OK] libunwind"

  # libcxxabi
  rm -rf libcxxabi-build
  mkdir -p libcxxabi-build
  cd libcxxabi-build

  sed -i 's|$<TARGET_LINKER_FILE:unwind_static>|'$SYSROOT/lib/libunwind.a'|' ../libcxxabi-10.0.1.src/src/CMakeLists.txt

  cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=$SYSROOT -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_AR=$AR \
    -DLIBCXXABI_LIBCXX_PATH=$WORKDIR/libcxx-10.0.1.src/ -DLLVM_PATH=$ROOT/llvm-typro \
    -DLIBCXXABI_LIBUNWIND_PATH=$WORKDIR/libunwind-10.0.1.src/ -DLIBCXXABI_LIBUNWIND_INCLUDES=$WORKDIR/libunwind-10.0.1.src/includes \
    -DLIBCXXABI_USE_LLVM_UNWINDER=On -DLIBCXXABI_ENABLE_STATIC_UNWINDER=On \
    -DLIBCXX_USE_COMPILER_RT=Off -DLIBCXXABI_USE_COMPILER_RT=Off \
    -DLIBCXXABI_INCLUDE_TESTS=Off \
    -DLIBCXX_ENABLE_RTTI=On -DLIBCXX_ENABLE_EXCEPTIONS=On -DLIBCXXABI_ENABLE_EXCEPTIONS=On \
    -DLIBCXX_HERMETIC_STATIC_LIBRARY=On -D=LIBCXXABI_HERMETIC_STATIC_LIBRARY=On -DLIBUNWIND_HERMETIC_STATIC_LIBRARY=On \
    -DCMAKE_CXX_COMPILER_WORKS=1 \
    ../libcxxabi-10.0.1.src
  # LIBCXXABI_TARGET_TRIPLE
  make -j$(nproc)
  make install
  cd ..
  echo "[OK] libcxxabi"

  # libcxx
  rm -rf libcxx-build
  mkdir -p libcxx-build
  cd libcxx-build

  export LDFLAGS="$LINK_FLAGS -static-libgcc -lgcc"

  cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=$SYSROOT -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_AR=$AR \
    -DLIBCXX_CXX_ABI=libcxxabi -DLIBCXX_CXX_ABI_INCLUDE_PATHS=$WORKDIR/libcxxabi-10.0.1.src/include \
    -DLIBCXX_CXX_ABI_LIBRARY_PATH=$SYSROOT/lib -DLIBCXX_ENABLE_STATIC_ABI_LIBRARY=On \
    -DLIBCXXABI_USE_LLVM_UNWINDER=On -DLIBCXXABI_ENABLE_STATIC_UNWINDER=On \
    -DLIBCXX_USE_COMPILER_RT=Off -DLIBCXXABI_USE_COMPILER_RT=Off \
    -DLIBCXX_HAS_MUSL_LIBC=On \
    -DLIBCXX_INCLUDE_TESTS=Off -DLIBCXX_INCLUDE_BENCHMARKS=Off \
    -DCMAKE_CXX_COMPILER_WORKS=1 \
    -DLIBCXX_ENABLE_RTTI=On -DLIBCXX_ENABLE_EXCEPTIONS=On -DLIBCXXABI_ENABLE_EXCEPTIONS=On \
    -DLIBCXX_HERMETIC_STATIC_LIBRARY=On -D=LIBCXXABI_HERMETIC_STATIC_LIBRARY=On -DLIBUNWIND_HERMETIC_STATIC_LIBRARY=On \
    ../libcxx-10.0.1.src
  make -j$(nproc)
  make install
  echo "[OK] libcxx"

  cp -rL /usr/lib/clang $SYSROOT/usr/lib/
}



make_compilerrt() {
  mkdir -p "$SYSROOT"
  WORKDIR="$SYSROOT-work"
  mkdir -p "$WORKDIR"
  cd "$WORKDIR"
  tar -xf $ROOT/sysroots/compiler-rt-10.0.1.src.tar.xz

  export CC=$SYSROOT/bin/my-clang
  export CXX=$SYSROOT/bin/my-clang++
  export AR=$CLANG_AR
  export RANLIB=$CLANG_RANLIB
  export CFLAGS="$FLAGS $DEFINITIONS -isystem $SYSROOT/usr/lib/gcc/x86_64-linux-gnu/10/include"
  export CXXFLAGS="$FLAGS $DEFINITIONS  -isystem $SYSROOT/usr/lib/gcc/x86_64-linux-gnu/10/include"
  export LDFLAGS="--sysroot $SYSROOT $LINK_FLAGS"

  rm -rf compilerrt-build
  mkdir -p compilerrt-build
  cd compilerrt-build

  cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_INSTALL_PREFIX=$SYSROOT -DCMAKE_CXX_COMPILER=$CXX \
      -DLLVM_CONFIG_PATH=$CLANG_CONFIG \
      -DCOMPILER_RT_BUILD_SANITIZERS=Off -DCOMPILER_RT_BUILD_XRAY=Off -DCOMPILER_RT_BUILD_LIBFUZZER=Off -DCOMPILER_RT_BUILD_PROFILE=Off \
      ../compiler-rt-10.0.1.src
    make -j$(nproc)
    make install
  cd ..
  echo "[OK] compilerrt"
}




make_openmp() {
  mkdir -p "$SYSROOT"
  WORKDIR="$SYSROOT-work"
  mkdir -p "$WORKDIR"
  cd "$WORKDIR"
  tar -xf $ROOT/sysroots/openmp-10.0.1.src.tar.xz
  sed -i 's|KMP_TDATA_GTID |KMP_TDATA_GTID_DISABLED |' openmp-10.0.1.src/runtime/src/kmp_config.h.cmake
  sed -i 's|#define KMP_DYNAMIC_LIB LIBOMP_ENABLE_SHARED|#define KMP_DYNAMIC_LIB 1|' openmp-10.0.1.src/runtime/src/kmp_config.h.cmake

  export CC=$SYSROOT/bin/my-clang
  export CXX=$SYSROOT/bin/my-clang++
  export AR=$CLANG_AR
  export RANLIB=$CLANG_RANLIB
  export CFLAGS="$FLAGS $DEFINITIONS -isystem $SYSROOT/usr/lib/clang/10.0.0/include"
  export CXXFLAGS="$FLAGS $DEFINITIONS  -isystem $SYSROOT/usr/lib/clang/10.0.0/include"
  export LDFLAGS="--sysroot $SYSROOT $LINK_FLAGS"

  rm -rf openmp-build
  mkdir -p openmp-build
  cd openmp-build

  cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=$SYSROOT -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX \
    -DENABLE_OMPT_TOOLS=Off -DLIBOMP_ENABLE_SHARED=Off \
    ../openmp-10.0.1.src
  make -j$(nproc)
  make install
  cd ..
  echo "[OK] openmp"
}




make_rt_libs () {
  if [ ! -d "$SYSROOT" ]; then
    echo "ERROR: $SYSROOT does not exist"
    return 1
  fi

  mkdir -p "$SYSROOT"
  WORKDIR="$SYSROOT-work"
  mkdir -p "$WORKDIR/rt"
  cd "$WORKDIR/rt"

  rm -rf "$SYSROOT/include/souffle"
  if [ -d /usr/local/include/souffle ]; then
    cp -r /usr/local/include/souffle "$SYSROOT/include/"
  else
    cp -r /usr/include/souffle "$SYSROOT/include/"
  fi

  export CC=$SYSROOT/bin/my-clang
  export CXX=$SYSROOT/bin/my-clang++
  export CFLAGS="$FLAGS $DEFINITIONS"
  export CXXFLAGS="$FLAGS $DEFINITIONS"
  export LDFLAGS="$FLAGS $LINK_FLAGS"

  cmake "$ROOT/llvm-typro/lib/Typegraph" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=$SYSROOT -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX \
    -DTYPRO_LIBS_ONLY=1 \
    -DOpenMP_C_FLAGS=-fopenmp -DOpenMP_C_LIB_NAMES=omp -DOpenMP_omp_LIBRARY=omp -DOpenMP_CXX_FLAGS=-fopenmp -DOpenMP_CXX_LIB_NAMES=omp
  #cmake "$ROOT/llvm-typro/lib/Typegraph" -DCMAKE_BUILD_TYPE=Debug -DTYPRO_LIBS_ONLY=1

  make -j$(nproc) typro-instrumentation-static typro-rt-static
  # install
  mkdir -p "$PROTECTED_SYSROOT/usr/typro-lib"
  cp libtypro-instrumentation-static.a "$PROTECTED_SYSROOT/usr/typro-lib/libtypro-instrumentation.a"
  cp libtypro-rt-static.a "$PROTECTED_SYSROOT/usr/typro-lib/libtypro-rt.a"
  cp "$SYSROOT/lib/libc++.a" "$SYSROOT/lib/libc++abi.a" "$SYSROOT/lib/libomp.a" "$SYSROOT/lib/libunwind.a" "$SYSROOT/lib/libarcher_static.a" "$PROTECTED_SYSROOT/usr/typro-lib/"
  cp "$SYSROOT/lib/librt.a" "$SYSROOT/lib/libdl.a" "$SYSROOT/lib/libpthread.a" "$PROTECTED_SYSROOT/usr/typro-lib/"

  # unpack typro-rt
  mkdir -p "$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj"
  ar --output="$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj" -x "$PROTECTED_SYSROOT/usr/typro-lib/libtypro-rt.a"
  # add stdlib stuff
  cp "$WORKDIR/musl-1.2.3/obj/src/stdlib/qsort_nr.lo" "$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj/qsort_nr.o"
  cp "$WORKDIR/musl-1.2.3/obj/src/stdlib/qsort.lo" "$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj/qsort.o"
  cp "$WORKDIR/musl-1.2.3/obj/src/signal/sigaction.lo" "$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj/sigaction.o"
  cp "$WORKDIR/musl-1.2.3/obj/src/signal/signal.lo" "$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj/signal.o"
  cp "$WORKDIR/musl-1.2.3/obj/src/thread/pthread_create.lo" "$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj/pthread_create.o"
  cp "$WORKDIR/musl-1.2.3/obj/src/thread/pthread_key_create.lo" "$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj/pthread_key_create.o"
  cp "$WORKDIR/musl-1.2.3/obj/src/thread/pthread_atfork.lo" "$PROTECTED_SYSROOT/usr/typro-lib/typro-rt-obj/pthread_atfork.o"

  # add a bit of additional libc to libtypro-instrumentation.a
  $CLANG_AR r "$PROTECTED_SYSROOT/usr/typro-lib/libtypro-instrumentation.a" "$WORKDIR/musl-1.2.3/obj/src/signal/signal.lo" "$WORKDIR/musl-1.2.3/obj/src/signal/sigaction.lo"
  $CLANG_RANLIB "$PROTECTED_SYSROOT/usr/typro-lib/libtypro-instrumentation.a"
}





# === Architecture-specific instructions ===

FLAGS="-fPIC"
LINK_FLAGS=""
TARGET=""
SYSROOT="$ROOT/sysroots/x86_64-linux-musl-ref"
PROTECTED_SYSROOT="$ROOT/sysroots/x86_64-linux-musl"
DEB_ARCH=amd64
DEB_TARGET=x86_64-linux-gnu
ARCH_MUSL=x86_64
download_dependencies
make_musl_libc
make_clang_scripts
make_libcxx
#make_compilerrt
make_openmp
make_rt_libs


FLAGS="--target=aarch64-linux-musl -fPIC"
LINK_FLAGS="-Wl,-z,notext"
TARGET="aarch64-linux-musl"
SYSROOT="$ROOT/sysroots/aarch64-linux-musl"
DEB_ARCH=arm64
DEB_TARGET=aarch64-linux-gnu
#download_dependencies
#make_musl_libc
#make_clang_scripts
#make_libcxx

SYSROOT="$ROOT/sysroots/aarch64-linux-gnu-ref"
#download_ref_debs



SYSROOT="$ROOT/sysroots/arm-linux-musleabihf"
FLAGS="--target=armv7-linux-musleabihf -mfloat-abi=hard -fPIC --gcc-toolchain=$SYSROOT/usr/lib/gcc/arm-linux-gnueabihf/10/"
LINK_FLAGS="-L$SYSROOT/usr/lib/gcc/arm-linux-gnueabihf/10/"
TARGET="arm-linux-musleabihf"
DEB_ARCH=armhf
DEB_TARGET=arm-linux-gnueabihf
#download_dependencies
#make_musl_libc
#make_clang_scripts
#make_libcxx


# arm-linux-musleabi/arm-linux-musleabihf
