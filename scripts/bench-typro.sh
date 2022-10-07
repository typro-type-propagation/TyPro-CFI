#!/usr/bin/env bash
# run in /spec/
# run: ~/bench-typro.sh 2>&1 | tee -a log/typro.log
# use a screen.

set -eu

#PARAMS="--noreportable -n 1 -i test 401.bzip2"
#PARAMS="--noreportable 400.perlbench 401.bzip2 403.gcc 433.milc 445.gobmk 456.hmmer 458.sjeng 464.h264ref 482.sphinx3"
PARAMS="--noreportable 401.bzip2"

bench_typro() {
	free && sync && echo 3 > /proc/sys/vm/drop_caches && free
	cset shield --exec -- ~/runspec-trampoline.sh -c spec-clang-lto-o3-typro-shielded.cfg $PARAMS
	date
}

bench_ref() {
	free && sync && echo 3 > /proc/sys/vm/drop_caches && free
	cset shield --exec -- ~/runspec-trampoline.sh -c spec-clang-lto-o3-typro-ref.cfg $PARAMS
	date
}


#sudo governor-set f
#echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
ulimit -s unlimited
free
swapoff -a
cset shield -c 1 -k on

echo ""
echo ""
echo ""
echo "========================================================================="

echo 'Ready...'
set -x
sleep 30

START_TIME="$(date '+%d.%m.%Y %H:%M:%S %Z')"

date


bench_typro
bench_ref
bench_typro
bench_ref
bench_typro
bench_ref
bench_typro
bench_ref
bench_typro
bench_ref

echo 'DONE.'

# Reset stuff
#sudo governor-set o
#echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
cset shield -r
