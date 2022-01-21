#!/usr/bin/env bash

set -e

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
ROOT=$(dirname "$SCRIPTPATH")



arches="arm64,aarch64-linux-gnu mips64el,mips64el-linux-gnuabi64"



cd "$ROOT/sysroots"
for arch in $arches; do
	IFS=","
	set $arch
	cd "$ROOT/sysroots/$2"

	if [ -L usr/include/souffle ]; then
		rm usr/include/souffle
	fi
	if [ -L usr/typro-lib ]; then
		rm usr/typro-lib
	fi

	ln -s /usr/include/souffle usr/include/souffle
	ln -s "../../runtime_libs/$2" usr/typro-lib
	unset IFS
done
