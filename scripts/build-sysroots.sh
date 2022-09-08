#!/usr/bin/env bash

set -eu

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
ROOT=$(dirname "$SCRIPTPATH")



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

download_and_install_deb() {
  package_name_for_regex="${1/+/\\+}"
  # download package index
  if [ ! -f /tmp/Packages-Sysroots-$DEB_ARCH ]; then
    wget "http://ftp.de.debian.org/debian/dists/bullseye/main/binary-$DEB_ARCH/Packages.gz" -O/tmp/Packages-Sysroots-$DEB_ARCH.gz
    gzip -d /tmp/Packages-Sysroots-$DEB_ARCH.gz
  fi
  # parse the package index
  url=$(grep -Pzo '(?s)Package: '"$package_name_for_regex"'\n.*?\n\n' /tmp/Packages-Sysroots-$DEB_ARCH | grep -Poa '^Filename: \K.*$' || true)
  if [ -z "$url" ]; then
    echo "Package not found: $1 ($DEB_ARCH)"
    return 1
  fi
  # download and install the package
  url="http://ftp.de.debian.org/debian/$url"
  echo "[+] Installing $(basename $url) from '$url' ..."
  install_deb "$url" "$(basename $url)"
}



download_sysroot() {
  #download_and_install_deb bash
  #download_and_install_deb dash
  #download_and_install_deb base-files
  #download_and_install_deb debianutils
  #download_and_install_deb gawk
  #return

  download_and_install_deb coreutils
  download_and_install_deb gcc-10-base
  download_and_install_deb libacl1
  download_and_install_deb libatomic1
  download_and_install_deb libattr1
  download_and_install_deb libboost-regex1.74.0
  download_and_install_deb libc-bin
  download_and_install_deb libc-dev-bin
  download_and_install_deb libc6
  download_and_install_deb libc6-dev
  download_and_install_deb libcrypt-dev
  download_and_install_deb libcrypt1
  download_and_install_deb libgcc-10-dev
  download_and_install_deb libgcc-s1
  download_and_install_deb libgmp-dev
  download_and_install_deb libgmp10
  download_and_install_deb libgmpxx4ldbl
  download_and_install_deb libgomp1
  download_and_install_deb libgssapi-krb5-2
  download_and_install_deb libmpc-dev
  download_and_install_deb libmpc3
  download_and_install_deb libmpfr-dev
  download_and_install_deb libmpfr6
  download_and_install_deb libnsl-dev
  download_and_install_deb libnsl2
  download_and_install_deb libomp-11-dev
  download_and_install_deb libomp-dev
  download_and_install_deb libomp5
  download_and_install_deb libomp5-11
  download_and_install_deb libreadline8
  download_and_install_deb libsigsegv2
  download_and_install_deb libstdc++-10-dev
  download_and_install_deb libstdc++6
  download_and_install_deb libstdc++6
  download_and_install_deb libtinfo6
  download_and_install_deb libtirpc-common
  download_and_install_deb libtirpc-dev
  download_and_install_deb libtirpc3
  download_and_install_deb libunwind-dev
  download_and_install_deb libunwind8
  download_and_install_deb linux-libc-dev
  download_and_install_deb readline-common
  download_and_install_deb systemtap-sdt-dev

  # dependencies for example programs
  download_and_install_deb libexpat1-dev
  download_and_install_deb libexpat1
}

fix_links() {
  pushd .
  cd "$SYSROOT"
  find "." -type l -lname '/*' -exec sh -c 'file="$0"; dir=$(dirname "$file"); target=$(readlink "$0"); prefix=$(dirname "$dir" | sed 's@[^/]*@\.\.@g'); newtarget="$prefix$target"; ln -snf $newtarget $file' {} \;

  if [ -L usr/include/souffle ]; then
  	rm usr/include/souffle
  fi
	if [ -L usr/typro-lib ]; then
		rm usr/typro-lib
	fi

 	if [ -d /usr/local/include/souffle ]; then
 		ln -s /usr/local/include/souffle usr/include/souffle
 	else
 		ln -s /usr/include/souffle usr/include/souffle
 	fi
 	ln -s "../../runtime_libs/$TARGET" usr/typro-lib

  popd
}



# === Architecture-specific instructions ===

TARGET=""
SYSROOT="$ROOT/sysroots/x86_64-linux-gnu"
DEB_ARCH=amd64
DEB_TARGET=x86_64-linux-gnu
download_sysroot
fix_links



TARGET="aarch64-linux-gnu"
SYSROOT="$ROOT/sysroots/aarch64-linux-gnu"
DEB_ARCH=arm64
DEB_TARGET=aarch64-linux-gnu
download_sysroot
fix_links



TARGET="mips64el-linux-gnuabi64"
SYSROOT="$ROOT/sysroots/mips64el-linux-gnuabi64"
DEB_ARCH=mips64el
DEB_TARGET=mips64el-linux-gnuabi64
download_sysroot
fix_links

