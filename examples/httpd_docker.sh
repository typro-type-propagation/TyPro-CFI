#!/bin/bash

set -e

httpd_v=2.4.51
apr_v=1.7.0
apr_util_v=1.6.1

source _lib.sh

# prepare test env - html files come from nginx script
mkdir -p $INSTALL_DIR/conf $INSTALL_DIR/cgi-bin $INSTALL_DIR/logs
cp -r httpd_config/* $INSTALL_DIR/conf/
sed -i "s|INSTALL_DIR|$INSTALL_DIR|" $INSTALL_DIR/conf/httpd.conf
echo '#!/bin/bash' > $INSTALL_DIR/cgi-bin/date
echo 'echo -e "Content-Type: text/html\n\n"' >> $INSTALL_DIR/cgi-bin/date
echo 'date' >> $INSTALL_DIR/cgi-bin/date
chmod +x $INSTALL_DIR/cgi-bin/date
cp $INSTALL_DIR/cgi-bin/date $INSTALL_DIR/cgi-bin/date.sh
if [ -f "$INSTALL_DIR/logs/apache_error.log" ]; then
    echo "=== Recompilation $(date) ===" >> $INSTALL_DIR/logs/apache_error.log
fi
# exit 0

# build pcre (if not existing)
$SCRIPT_DIR/lib_pcre.sh $MODE lazy

cd $SCRIPT_DIR
mkdir -p httpd
cd httpd

if [ ! -d httpd-${httpd_v}-$MODE ]; then
    if [ ! -f httpd-${httpd_v}.tar.gz ]; then
        wget https://dlcdn.apache.org//httpd/httpd-${httpd_v}.tar.gz
    fi
    tar -xf httpd-${httpd_v}.tar.gz
    mv httpd-${httpd_v} httpd-${httpd_v}-$MODE
fi

cd httpd-${httpd_v}-$MODE/srclib

if [ ! -d apr ]; then
    if [ ! -f apr-${apr_v}.tar.gz ]; then
        wget https://dlcdn.apache.org//apr/apr-${apr_v}.tar.gz
    fi
    tar -xf apr-${apr_v}.tar.gz
    mv apr-${apr_v} apr
fi

if [ ! -d apr-util ]; then
    if [ ! -f apr-util-${apr-util_v}.tar.gz ]; then
        wget https://dlcdn.apache.org//apr/apr-util-${apr_util_v}.tar.gz
    fi
    tar -xf apr-util-${apr_util_v}.tar.gz
    mv apr-util-${apr_util_v} apr-util
fi

cd $SCRIPT_DIR/httpd/httpd-${httpd_v}-$MODE

case "$MODE" in
    "arm64_ref" | "arm64_enforcestatic" )
        CONFIGURE_FLAGS="$CONFIGURE_FLAGS ac_cv_file__dev_zero=yes ac_cv_func_setpgrp_void=yes apr_cv_tcp_nodelay_with_cork=yes ap_cv_void_ptr_lt_long=8"
        CFLAGS="$CFLAGS -DAPR_IOVEC_DEFINED"
        ;;
esac

./configure --prefix=$INSTALL_DIR --enable-static --disable-shared --with-pcre=$INSTALL_DIR/bin/pcre-config --with-included-apr --without-ssl --enable-rewrite --enable-cgi --enable-mods-static=few $CONFIGURE_FLAGS

rm -rf graph.*
nice -5 make -j$(nproc) 2>&1 | tee $SCRIPT_DIR/logs/httpd-$MODE.log

echo "[DONE]"
echo "To test, run:"
echo " cd $SCRIPT_DIR/httpd/httpd-${httpd_v}-$MODE"
echo " ./httpd -DFOREGROUND"
echo "curl 'http://localhost:12346/'"
echo "curl 'http://localhost:12346/index.html'"
echo "curl 'http://localhost:12346/texts'"
echo "curl 'http://localhost:12346/texts/'"
echo "curl 'http://localhost:12346/texts/test.txt'"
echo "curl 'http://localhost:12346/cgi-bin/date'"
echo "curl 'http://localhost:12346/cgi-bin/date.sh'"
echo "curl 'http://localhost:12346/' --header 'HTTP_PROXY: 127.0.0.1'"
