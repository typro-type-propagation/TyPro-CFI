#!/usr/bin/env bash

# partially taken from https://xw.is/wiki/Create_Debian_sysroots

set -e

# run as root.
# dependencies: apt install -y binfmt-support qemu qemu-user qemu-user-static debootstrap


SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
ROOT=$(dirname "$SCRIPTPATH")

mkdir -p /tmp/sysroots
cd /tmp/sysroots

#arches="armhf,arm-linux-gnueabihf amd64,x86_64-linux-gnu arm64,aarch64-linux-gnu mips,mips-linux-gnu mips64el,mips64el-linux-gnuabi64 ppc64el,powerpc64le-linux-gnu i386,i386-linux-gnu"
arches="arm64,aarch64-linux-gnu mips64el,mips64el-linux-gnuabi64"
packages="gdb,systemtap-sdt-dev,libgmp-dev,libmpfr-dev,libmpc-dev,libunwind-dev,libstdc++6,libgomp1,libboost-regex1.74.0,libomp-dev"

# Create sysroot in /tmp
for arch in $arches; do
	IFS=","
	set $arch
	echo "==============================================================================="
	echo "Creating sysroot for architecture $1  ($2)"
	echo "==============================================================================="
	rm -rf "$2"
	qemu-debootstrap --arch "$1" --variant=buildd stable "$2" http://deb.debian.org/debian --include="$packages"
	unset IFS
done

# Fix absolute links
for arch in $arches; do
	IFS=","
	set $arch
	find "$2" -type l -lname '/*' -exec sh -c 'file="$0"; dir=$(dirname "$file"); target=$(readlink "$0"); prefix=$(dirname "$dir" | sed 's@[^/]*@\.\.@g'); newtarget="$prefix$target"; ln -snf $newtarget $file' {} \;
	echo "[OK] Corrected links for $1 / $2."
	unset IFS
done

# Pack sysroots to archive in $ROOT/sysroots
mkdir -p "$ROOT/sysroots"
for arch in $arches; do
	IFS=","
	set $arch
	tar -c "$2/usr/include" "$2/usr/lib" "$2"/lib* "$2"/etc/ld* | xz -T 0 > "$ROOT/sysroots/sysroot-$2.tar.xz"
	echo "[OK] Created sysroot-$2.tar.xz ."
	tar -c "$2/bin" "$2/usr/bin" "$2/etc" "$2/sbin" "$2/usr/sbin" "$2/usr/share" | xz -T 0 > "$ROOT/sysroots/sysroot-$2-binaries.tar.xz"
	echo "[OK] Created sysroot-$2-binaries.tar.xz ."
	unset IFS
done


# Extract sysroots (for tests)
cd "$ROOT/sysroots"
for arch in $arches; do
	IFS=","
	set $arch
	tar -xf "$ROOT/sysroots/sysroot-$2.tar.xz"
	echo "[OK] Extracted sysroot-$2.tar.xz ."
	unset IFS
done
