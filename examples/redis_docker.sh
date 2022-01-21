#!/bin/bash

set -e
set -o pipefail

source _lib.sh

redis_v=6.2.6

cd $SCRIPT_DIR
mkdir -p redis
cd redis

if [ ! -d redis-$MODE ]; then
    #git clone --single-branch --depth 1 https://github.com/redis/redis.git redis-$MODE
    test -f "redis-${redis_v}.tar.gz" || wget "https://download.redis.io/releases/redis-${redis_v}.tar.gz"
    tar -xf redis-${redis_v}.tar.gz
    mv "redis-${redis_v}" redis-$MODE
fi

cd redis-$MODE


export USE_SYSTEMD="no"
export BUILD_TLS="no"
export PREFIX="$INSTALL_DIR"

echo "USE_SYSTEMD=$USE_SYSTEMD" >> src/.make-settings
echo "BUILD_TLS=$BUILD_TLS" >> src/.make-settings
echo "PREFIX=$PREFIX" >> src/.make-settings
echo "CFLAGS=$CFLAGS" >> src/.make-settings
echo "LDFLAGS=$LDFLAGS" >> src/.make-settings
echo "CC=$CC" >> src/.make-settings

rm src/Makefile
cp $SCRIPT_DIR/redis_Makefile src/Makefile

cat - tests/modules/Makefile > tests/modules/Makefile.tmp <<EOF
CC=$CC
LD=$CC
CFLAGS=$CFLAGS
LDFLAGS=$LDFLAGS
EOF
mv tests/modules/Makefile.tmp tests/modules/Makefile

cat - src/modules/Makefile > src/modules/Makefile.tmp <<EOF
CC=$CC
LD=$CC
CFLAGS=$CFLAGS
LDFLAGS=$LDFLAGS
EOF
mv src/modules/Makefile.tmp src/modules/Makefile

rm -rf graph.*
#nice -5 make -j$(nproc)
nice -5 make -j2 2>&1 | tee $SCRIPT_DIR/logs/redis-$MODE.log
cd tests/modules
nice -5 make -j2 2>&1 | tee -a $SCRIPT_DIR/logs/redis-$MODE.log

echo "[DONE MAKE]"

#echo "apt install tcl-dev (if not installed)"

#make test >out 2>&1
#OR:
#./runtest

# Enforced version:
# TG_CFI_OUTPUT_RT=auto TG_CFI_OUTPUT_RT_BASE=$(pwd)/src/redis-server ./runtest
# Instrumented version:
# TG_RT_SILENT=1 TG_RT_CALLTARGET_BASE=$(pwd)/src/redis-server ./runtest

#echo "[DONE TEST]"

echo "To run tests:"
echo "  cd $SCRIPT_DIR/redis/redis-$MODE"
echo "  ./runtest"