#!/bin/bash

# do once
# apt-get install libxml2-dev zlib1g-dev libbz2-dev libxslt1-dev

set -e
set -o pipefail

nginx_v=1.20.0
pcre_v=8.45
zlib_v=1.2.11

source _lib.sh
./lib_pcre.sh $MODE lazy
./lib_zlib.sh $MODE lazy

# building test stuff
mkdir -p "$INSTALL_DIR/logs" "$INSTALL_DIR/run" "$INSTALL_DIR/html/texts"
rm -f "$INSTALL_DIR/nginx.conf"
sed "s|INSTALL_DIR|$INSTALL_DIR|" "$SCRIPT_DIR/nginx.conf" > "$INSTALL_DIR/nginx.conf"
echo '<h1>Hello World!</h1>' > $INSTALL_DIR/html/index.html
echo 'Hello World!' > $INSTALL_DIR/html/texts/test.txt


cd $SCRIPT_DIR
mkdir -p nginx
cd nginx

if [ ! -d nginx-release-${nginx_v}-$MODE ]; then
    if [ ! -f release-${nginx_v}.tar.gz ]; then
        wget https://github.com/nginx/nginx/archive/refs/tags/release-${nginx_v}.tar.gz
    fi
    tar -xf release-${nginx_v}.tar.gz
    mv nginx-release-${nginx_v} nginx-release-${nginx_v}-$MODE
fi

cd nginx-release-${nginx_v}-$MODE

auto/configure --prefix=$INSTALL_DIR --sbin-path=./sbin/nginx --conf-path=./nginx.conf --pid-path=./run/nginx.pid \
    --with-cc="$WRAPPER $CLANG_ROOT/bin/clang" \
    --with-cc-opt="$CFLAGS" \
    --with-ld-opt="$LDFLAGS" \
    --with-cpu-opt=generic \
    --with-pcre=$SCRIPT_DIR/lib_pcre/pcre-${pcre_v}-$MODE \
    --with-zlib=$SCRIPT_DIR/lib_zlib/zlib-${zlib_v}-$MODE \
    --with-pcre-opt="$CFLAGS" \
    --with-zlib-opt="$CFLAGS" \
    --with-file-aio \
    --without-poll_module \
    --without-select_module \
    --with-http_v2_module \
    --without-http_access_module \
    --without-http_auth_basic_module \
    --without-http_autoindex_module \
    --without-http_browser_module \
    --without-http_charset_module \
    --without-http_empty_gif_module \
    --without-http_geo_module \
    --without-http_grpc_module \
    --without-http_memcached_module \
    --without-http_map_module \
    --without-http_ssi_module \
    --without-http_split_clients_module \
    --without-http_fastcgi_module \
    --without-http_uwsgi_module \
    --without-http_userid_module \
    --without-http_scgi_module \
    --without-mail_pop3_module \
    --without-mail_imap_module \
    --without-mail_smtp_module \
#    --with-openssl=$SCRIPT_DIR/nginx/openssl-${open_ssl_v} \
#    --with-http_ssl_module

# #./configure \
# auto/configure --prefix=$SCRIPT_DIR/nginx/install \
#             --with-cc="$CLANG_ROOT/bin/clang" \
#             --with-cpp="$CLANG_ROOT/bin/clang-cpp" \
#             --with-cc-opt="-O3 -flto -static" \
#             --with-ld-opt="-static" --with-cpu-opt=generic --with-pcre \
#             --with-debug \
#             --with-mail --with-poll_module --with-select_module \
#             --with-select_module --with-poll_module \
#             --with-http_v2_module --with-http_realip_module \
#             --with-http_addition_module --with-http_sub_module --with-http_dav_module \
#             --with-http_flv_module --with-http_mp4_module --with-http_gunzip_module \
#             --with-http_gzip_static_module --with-http_auth_request_module \
#             --with-http_random_index_module --with-http_secure_link_module \
#             --with-http_degradation_module --with-http_stub_status_module \
#             --with-mail 
#             #--with-http_ssl_module \
#             #--with-mail_ssl_module --with-openssl=$SCRIPT_DIR/openssl-1.1.1l

rm -rf graph.*
nice -5 make -j4 2>&1 | tee $SCRIPT_DIR/logs/nginx-$MODE.log

echo "[DONE MAKE]"

#make install

#../build/nginx-1.20.0/sbin/nginx

#echo "[DONE TESTS]"

echo "Start server:"
echo "  cd $SCRIPT_DIR/nginx/nginx-release-${nginx_v}-$MODE/objs"
#echo "export TG_RT_CALLTARGET_BASE=/tmp/nginx/nginx"
echo "  ./nginx"
echo "Test server:"
echo "curl 'http://localhost:8000/' -o -"
echo "curl 'http://localhost:8000/texts/test.txt' -o -"
echo "curl 'http://localhost:8001/' -o -"
echo "curl 'http://localhost:8001/test.txt' -o -"
echo "curl 'http://localhost:8001/test.txt' -o - --compressed -v"
echo "curl 'http://localhost:8001/test2.txt' -o - --compressed -v"
echo "curl 'http://localhost:8000/try/abc' -o -"
echo "curl 'http://localhost:8000/shm' -o -"
echo "curl 'http://localhost:8001/test.txt' -o - -r 0-2"
echo "curl 'http://localhost:8001/test.txt' -o - --http2"
