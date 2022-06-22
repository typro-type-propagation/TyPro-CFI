#!/bin/bash

set -e

source _lib.sh

cd $SCRIPT_DIR
mkdir -p pureftpd
cd pureftpd

DIR="pureftpd-$MODE"
if [ ! -d "$DIR" ]; then
    git clone https://github.com/jedisct1/pure-ftpd.git "$DIR"
fi

cd "$DIR"

./autogen.sh --enable-static --disable-shared
./configure --enable-static --disable-shared --without-capabilities --prefix=$INSTALL_DIR $CONFIGURE_FLAGS

rm -rf graph.*
nice -5 make -j$(nproc)

# recompile main binary to get right output files
rm -f src/pure-ftpd
make

echo "[DONE]"

echo "no official tests, but it runs, try here"

# OUTPUTS:
# pure-ftpd/pure-ftpd-instrument/src/*.json
# pure-ftpd/pure-ftpd-enforce/src/tgcfi.json
# pure-ftpd/pure-ftpd-icall/src/ifcc.json

echo "Prepare tests with:"
echo "  ./pureftpd_prepare_tests.sh"
echo "Start server with: (in directory '$SCRIPT_DIR/pureftpd/$DIR/src' )"
echo "  sudo ./pure-ftpd ../../../pure-ftpd.conf"
echo "Test server:"
echo "  curl ftp://localhost:2121/ -l"
echo "  curl ftp://localhost:2121/"
echo "  curl ftp://localhost:2121/test.txt"
echo "  curl -T /etc/passwd ftp://localhost:2121/ftp.test"
echo "  curl --user $(id -un) ftp://localhost:2121/   # use the real password"
echo "  curl --user $(id -un):abc ftp://localhost:2121/"
echo "  python3 -u ../../../pureftpd-tests.py"

exit 0

## PREPARE
sudo useradd -d /var/ftp -m -r -s /usr/sbin/nologin ftp
date | sudo tee /var/ftp/test.txt
sudo chown root:root /var/ftp/test.txt
sudo mkdir -p /var/ftp/a/b/c/d/e/f
echo HIHIHI | sudo tee /var/ftp/a/b/c/d/e/f/test1.txt
sudo mkdir -p /var/ftp/tmp
sudo mkdir -p /var/ftp/var/ftp
sudo chmod 0777 /var/ftp/tmp/
sudo ln -s /tmp /var/ftp/var/ftp/tmp
## START
sudo ./pure-ftpd ../../../pure-ftpd.conf
TG_RT_CALLTARGET_BASE=/var/ftp/tmp/pure-ftpd sudo --preserve-env=TG_RT_CALLTARGET_BASE ./pure-ftpd ../../../pure-ftpd.conf
## TEST
curl ftp://localhost:2121/ -l
curl ftp://localhost:2121/
curl ftp://localhost:2121/test.txt
curl -T /etc/passwd ftp://localhost:2121/ftp.test
curl --user $(id -un) ftp://localhost:2121/
curl --user $(id -un):abc ftp://localhost:2121/
python3 -u ../../../pureftpd-tests.py
## STATS
cp /var/ftp/tmp/pure-ftpd.calltargets* ./
