#!/bin/bash

set -e

sudo useradd -d /var/ftp -m -r -s /usr/sbin/nologin ftp
date | sudo tee /var/ftp/test.txt
sudo chown root:root /var/ftp/test.txt
sudo mkdir -p /var/ftp/a/b/c/d/e/f
echo HIHIHI | sudo tee /var/ftp/a/b/c/d/e/f/test1.txt
sudo mkdir -p /var/ftp/tmp
sudo mkdir -p /var/ftp/var/ftp
sudo chmod 0777 /var/ftp/tmp/
sudo ln -s /tmp /var/ftp/var/ftp/tmp

echo "Prepared ftp directories."

## START
#sudo ./pure-ftpd ../../../pure-ftpd.conf
#TG_RT_CALLTARGET_BASE=/var/ftp/tmp/pure-ftpd sudo --preserve-env=TG_RT_CALLTARGET_BASE ./pure-ftpd ../../../pure-ftpd.conf
## TEST
#curl ftp://localhost:2121/ -l
#curl ftp://localhost:2121/
#curl ftp://localhost:2121/test.txt
#curl -T /etc/passwd ftp://localhost:2121/ftp.test
#curl --user $(id -un) ftp://localhost:2121/
#curl --user $(id -un):abc ftp://localhost:2121/
#python3 -u ../../../pureftpd-tests.py
## STATS
#cp /var/ftp/tmp/pure-ftpd.calltargets* ./
