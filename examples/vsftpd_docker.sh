#!/bin/bash

set -e

vsftpd_v=3.0.5

source _lib.sh

cd $SCRIPT_DIR
mkdir -p vsftpd
cd vsftpd

if [ ! -d vsftpd-${vsftpd_v}-$MODE ]; then
    if [ ! -f vsftpd-${vsftpd_v}.tar.gz ]; then
        wget https://security.appspot.com/downloads/vsftpd-${vsftpd_v}.tar.gz
    fi
    tar -xf vsftpd-${vsftpd_v}.tar.gz
    mv vsftpd-${vsftpd_v} vsftpd-${vsftpd_v}-$MODE
fi

cd vsftpd-${vsftpd_v}-$MODE

rm Makefile
echo "CC = $CC" >> Makefile
echo "CFLAGS_BASE = $CFLAGS" >> Makefile
echo "LDFLAGS_BASE = $LDFLAGS" >> Makefile
cat $SCRIPT_DIR/vsftpd_Makefile >> Makefile

#./configure --enable-static --disable-shared

rm -rf graph.*
nice -5 make

echo "[DONE]"

echo "no official tests, but it runs, try here"
echo "cd $SCRIPT_DIR/vsftpd/vsftpd-${vsftpd_v}-$MODE"
echo "./vsftpd ../../vsftpd.conf"
echo "curl -l ftp://localhost:2121"
echo "curl ftp://localhost:2121/etc/passwd"
echo "curl -T /etc/passwd ftp://localhost:2121/tmp/ftp.test"
