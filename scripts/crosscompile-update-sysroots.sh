#!/usr/bin/env bash

set -eu

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
ROOT=$(dirname "$SCRIPTPATH")
CLANG_BIN="$ROOT/build/bin"
if [ "$ROOT/llvm-typro/cmake-build-minsizerel/bin/clang" -nt "$CLANG_BIN/clang" ]; then
  CLANG_BIN=$ROOT/llvm-typro/cmake-build-minsizerel/bin
fi
echo "Clang directory: $CLANG_BIN"

cd $ROOT/sysroots





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





download_debs() {
  install_deb "http://ftp.de.debian.org/debian/pool/main/p/pam/libpam0g-dev_1.4.0-9+deb11u1_$DEB_ARCH.deb" "libpam0g-dev_1.4.0-9+deb11u1_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/p/pam/libpam0g_1.4.0-9+deb11u1_$DEB_ARCH.deb" "libpam0g_1.4.0-9+deb11u1_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/libc/libcap2/libcap-dev_2.44-1_$DEB_ARCH.deb" "libcap-dev_2.44-1_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/libc/libcap2/libcap2_2.44-1_$DEB_ARCH.deb" "libcap2_2.44-1_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/e/expat/libexpat1-dev_2.2.10-2+deb11u3_$DEB_ARCH.deb" "libexpat1-dev_2.2.10-2+deb11u3_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/e/expat/libexpat1_2.2.10-2+deb11u3_$DEB_ARCH.deb" "libexpat1_2.2.10-2+deb11u3_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/u/util-linux/libuuid1_2.36.1-8+deb11u1_$DEB_ARCH.deb" "libuuid1_2.36.1-8+deb11u1_$DEB_ARCH.deb"
  install_deb "http://ftp.de.debian.org/debian/pool/main/u/util-linux/uuid-dev_2.36.1-8+deb11u1_$DEB_ARCH.deb" "uuid-dev_2.36.1-8+deb11u1_$DEB_ARCH.deb"

  echo "[OK] Installed additional packages"
}




SYSROOT="$ROOT/sysroots/aarch64-linux-gnu"
DEB_ARCH=arm64
DEB_TARGET=aarch64-linux-gnu
download_debs
