#!/bin/bash

set -e
set -o pipefail

lighttpd_v=1.4.59
open_ssl_v=1.1.1l

source _lib.sh

$SCRIPT_DIR/lib_zlib.sh $MODE lazy
$SCRIPT_DIR/lib_pcre.sh $MODE lazy

cd $SCRIPT_DIR
mkdir -p lighttpd
cd lighttpd

if [ ! -d lighttpd-${lighttpd_v}-$MODE ]; then
    if [ ! -f lighttpd-${lighttpd_v}.tar.gz ]; then
        wget https://download.lighttpd.net/lighttpd/releases-1.4.x/lighttpd-${lighttpd_v}.tar.gz
    fi
    tar -xf lighttpd-${lighttpd_v}.tar.gz
    mv lighttpd-${lighttpd_v} lighttpd-${lighttpd_v}-$MODE
fi

# if [ ! -d openssl-${open_ssl_v} ]; then
#     if [ ! -f openssl-${open_ssl_v}.tar.gz ]; then
#         wget https://www.openssl.org/source/openssl-${open_ssl_v}.tar.gz
#     fi
#     tar -xf openssl-${open_ssl_v}.tar.gz
# fi

cd lighttpd-${lighttpd_v}-$MODE

# ./configure --enable-static --without-zlib --without-bzip2 --without-pcre --without-pic --disable-shared 

# if [ ! -f "src/plugin-static.h" ]; then
#   cat >"src/plugin-static.h" <<EOF
# PLUGIN_INIT(mod_alias)

# PLUGIN_INIT(mod_extforward)

# PLUGIN_INIT(mod_access)
# PLUGIN_INIT(mod_auth) /* CRYPT LDAP LBER */

# PLUGIN_INIT(mod_setenv)

# #ifdef HAVE_LUA
# PLUGIN_INIT(mod_magnet) /* LUA */
# #endif
# PLUGIN_INIT(mod_flv_streaming)

# /* * indexfile must come before dirlisting for dirlisting not to override */

# PLUGIN_INIT(mod_indexfile)
# PLUGIN_INIT(mod_userdir)
# PLUGIN_INIT(mod_dirlisting)

# PLUGIN_INIT(mod_status)

# PLUGIN_INIT(mod_simple_vhost)
# PLUGIN_INIT(mod_evhost)

# PLUGIN_INIT(mod_secdownload)

# PLUGIN_INIT(mod_cgi)
# PLUGIN_INIT(mod_fastcgi)
# PLUGIN_INIT(mod_scgi)
# PLUGIN_INIT(mod_proxy)

# /* staticfile must come after cgi/ssi/et al. */
# PLUGIN_INIT(mod_staticfile)

# #ifdef HAVE_LUA
# //PLUGIN_INIT(mod_cml) /* MEMCACHE LUA LUALIB */
# #endif

# PLUGIN_INIT(mod_webdav) /* XML2 SQLITE3 UUID */

# /* post-processing modules */
# PLUGIN_INIT(mod_evasive)
# PLUGIN_INIT(mod_usertrack)
# PLUGIN_INIT(mod_expire)
# //PLUGIN_INIT(mod_rrdtool)
# PLUGIN_INIT(mod_accesslog)
# EOF
# fi

 # cmake -DBUILD_STATIC:BOOL=ON \
 #    -DBUILD_FULLSTATIC:BOOL=ON \
 #    -DCMAKE_VERBOSE_MAKEFILE=OFF \
 #    -DBUILD_DYNAMIC:BOOL=OFF \
 #    -DWITH_OPENSSL:BOOL=OFF \
 #    -DWITH_BZIP:BOOL=OFF \
 #    -DWITH_MYSQL:BOOL=OFF \
 #    -DWITH_SQLITE3:BOOL=OFF \
 #    -DWITH_PCRE:BOOL=OFF \
 #    -DWITH_WEBDAV:BOOL=OFF \
 #    -DWITH_GZIP:BOOL=OFF \
 #    -DWITH_BZIP@:BOOL=OFF \
 #    -DWITH_MEMCACHE:BOOL=OFF \
 #    -DWITH_XML:BOOL=OFF \
 #    -DCMAKE_INSTALL_PREFIX:PATH=/tmp/lighttpd/ .

export LIGHTTPD_STATIC=yes
cp $SCRIPT_DIR/lighttpd_plugin-static.h src/plugin-static.h

./configure --enable-static --disable-shared \
    --with-pcre=$INSTALL_DIR \
    --with-zlib=$INSTALL_DIR \
    --without-bzip2 --without-pic --without-openssl $CONFIGURE_FLAGS

rm -rf graph.*
nice -5 make -j$(nproc)

echo "[DONE MAKE]"

# cd $SCRIPT_DIR/lighttpd/lighttpd-1.4.59-enforce/src
# ./lighttpd -f ../../../lighttpd_sample.conf -D

#cd tests
#make check

echo "[DONE TESTS]"
echo "To re-run tests:"
echo "  cd $SCRIPT_DIR/lighttpd/lighttpd-${lighttpd_v}-$MODE/tests"
echo "  make check"
