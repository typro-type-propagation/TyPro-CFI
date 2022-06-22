#!/usr/bin/env bash

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
BASE_DIR="$( cd "$( dirname "$SCRIPT_DIR" )" &> /dev/null && pwd )"
cd $SCRIPT_DIR

# copy httpd to local
#
#
#
#


SYSROOT="$(dirname $SCRIPT_DIR)/sysroots/aarch64-linux-gnu"
TARGET="aarch64-linux-gnu"
GCC_VERSION="10"
export TARGET_FLAGS="--sysroot=$SYSROOT -m aarch64linux -mattr=+fp16fml+fullfp16+sha3+v8.5a+neon+fp-armv8+crypto"



cd httpd/httpd-2.4.51-arm64_enforcestatic
set -x

$BASE_DIR/examples/settings_enforcestatic.sh \
	"$BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/ld.lld" \
	$TARGET_FLAGS \
	-EL --eh-frame-hdr -m aarch64linux -dynamic-linker /lib/ld-linux-aarch64.so.1 -o httpd \
	$SYSROOT/usr/lib/$TARGET/crt1.o $SYSROOT/usr/lib/$TARGET/crti.o $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/crtbegin.o \
	-L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION -L$SYSROOT/usr/lib/$TARGET -L$SYSROOT/lib/$TARGET -L$SYSROOT/lib -L$SYSROOT/usr/lib \
	-$LBASE_DIR/examples/install_arm64_enforcestatic/lib -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/../../.. -$LBASE_DIR/llvm-typro/cmake-build-minsizerel/bin/../lib \
	-plugin $BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/../lib/LLVMgold.so -plugin-opt=mcpu=generic -plugin-opt=O3 -rpath $BASE_DIR/examples/install_arm64_enforcestatic/lib modules.o buildmark.o --export-dynamic \
	server/.libs/libmain.a modules/aaa/.libs/libmod_authn_file.a modules/aaa/.libs/libmod_authn_core.a modules/aaa/.libs/libmod_authz_host.a modules/aaa/.libs/libmod_authz_groupfile.a modules/aaa/.libs/libmod_authz_user.a modules/aaa/.libs/libmod_authz_core.a modules/aaa/.libs/libmod_access_compat.a modules/aaa/.libs/libmod_auth_basic.a modules/core/.libs/libmod_so.a modules/filters/.libs/libmod_reqtimeout.a modules/filters/.libs/libmod_filter.a modules/http/.libs/libmod_http.a modules/http/.libs/libmod_mime.a modules/loggers/.libs/libmod_log_config.a modules/metadata/.libs/libmod_env.a modules/metadata/.libs/libmod_headers.a modules/metadata/.libs/libmod_setenvif.a modules/metadata/.libs/libmod_version.a modules/arch/unix/.libs/libmod_unixd.a modules/generators/.libs/libmod_status.a modules/generators/.libs/libmod_autoindex.a modules/generators/.libs/libmod_cgi.a modules/mappers/.libs/libmod_dir.a modules/mappers/.libs/libmod_alias.a modules/mappers/.libs/libmod_rewrite.a server/mpm/event/.libs/libevent.a os/unix/.libs/libos.a $BASE_DIR/examples/install_arm64_enforcestatic/lib/libpcre.a $BASE_DIR/examples/httpd/httpd-2.4.51-arm64_enforcestatic/srclib/apr-util/.libs/libaprutil-1.a -lexpat $BASE_DIR/examples/httpd/httpd-2.4.51-arm64_enforcestatic/srclib/apr/.libs/libapr-1.a \
	-luuid -lcrypt -lpthread -ldl -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed \
	$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/crtend.o $SYSROOT/usr/lib/$TARGET/crtn.o

set +x
cd ../..

cd httpd/httpd-2.4.51-arm64_ref
set -x

$BASE_DIR/examples/settings_ref.sh \
	"$BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/ld.lld" \
	$TARGET_FLAGS \
	-EL --eh-frame-hdr -m aarch64linux -dynamic-linker /lib/ld-linux-aarch64.so.1 -o httpd \
	$SYSROOT/usr/lib/$TARGET/crt1.o $SYSROOT/usr/lib/$TARGET/crti.o $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/crtbegin.o \
	-L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION -L$SYSROOT/usr/lib/$TARGET -L$SYSROOT/lib/$TARGET -L$SYSROOT/lib -L$SYSROOT/usr/lib \
	-$LBASE_DIR/examples/install_arm64_ref/lib -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/../../.. -$LBASE_DIR/llvm-typro/cmake-build-minsizerel/bin/../lib \
	-plugin $BASE_DIR/llvm-typro/cmake-build-minsizerel/bin/../lib/LLVMgold.so -plugin-opt=mcpu=generic -plugin-opt=O3 -rpath $BASE_DIR/examples/install_arm64_ref/lib modules.o buildmark.o --export-dynamic \
	server/.libs/libmain.a modules/aaa/.libs/libmod_authn_file.a modules/aaa/.libs/libmod_authn_core.a modules/aaa/.libs/libmod_authz_host.a modules/aaa/.libs/libmod_authz_groupfile.a modules/aaa/.libs/libmod_authz_user.a modules/aaa/.libs/libmod_authz_core.a modules/aaa/.libs/libmod_access_compat.a modules/aaa/.libs/libmod_auth_basic.a modules/core/.libs/libmod_so.a modules/filters/.libs/libmod_reqtimeout.a modules/filters/.libs/libmod_filter.a modules/http/.libs/libmod_http.a modules/http/.libs/libmod_mime.a modules/loggers/.libs/libmod_log_config.a modules/metadata/.libs/libmod_env.a modules/metadata/.libs/libmod_headers.a modules/metadata/.libs/libmod_setenvif.a modules/metadata/.libs/libmod_version.a modules/arch/unix/.libs/libmod_unixd.a modules/generators/.libs/libmod_status.a modules/generators/.libs/libmod_autoindex.a modules/generators/.libs/libmod_cgi.a modules/mappers/.libs/libmod_dir.a modules/mappers/.libs/libmod_alias.a modules/mappers/.libs/libmod_rewrite.a server/mpm/event/.libs/libevent.a os/unix/.libs/libos.a $BASE_DIR/examples/install_arm64_ref/lib/libpcre.a $BASE_DIR/examples/httpd/httpd-2.4.51-arm64_ref/srclib/apr-util/.libs/libaprutil-1.a -lexpat $BASE_DIR/examples/httpd/httpd-2.4.51-arm64_ref/srclib/apr/.libs/libapr-1.a \
	-luuid -lcrypt -lpthread -ldl -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed \
	$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/crtend.o $SYSROOT/usr/lib/$TARGET/crtn.o

set +x
cd ../..

# copy back
#
#
