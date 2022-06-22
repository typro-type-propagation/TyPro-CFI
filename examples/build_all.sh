#!/usr/bin/env bash

set -e
set -o pipefail

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd $SCRIPT_DIR
mkdir -p logs

LOGFILE=$SCRIPT_DIR/logs/build-all.log

PROGRAMS="vsftpd pureftpd lighttpd httpd memcached redis nginx"
# PROGRAMS="vsftpd pureftpd"
# PROGRAMS="nginx"
LIBRARY_DIRS="libevent lib_zlib lib_pcre"



function build {
    echo "=== BUILDING $1 ($2) ==="
    cd $SCRIPT_DIR
    (./$1_docker.sh $2 2>&1 | tee "logs/build-all-$1-$2.log") && \
        echo "$(date) | Success: $1 ($2)" >> "$LOGFILE" || \
        echo "$(date) | Failure: $1 ($2)" >> "$LOGFILE"
}



function cleanup {
    for P in $PROGRAMS; do
        rm -rf "$P/"
    done
    for P in $LIBRARY_DIRS; do
        rm -rf "$P/"
    done
}




MODE="${1:-enforce}"  # MODE: enforce, instrument, icall, ref
# this wrapper sets all TG_ settings (also for recompilations)
case "$MODE" in
    "clean")
        cleanup
        ;;
    "all")
        echo "Mode = $MODE"
        echo "$(date) | --- Start building with mode = $MODE ---" >> $LOGFILE
        for P in $PROGRAMS; do
            build "$P" "ref"
            build "$P" "instrument"
            build "$P" "enforce"
            build "$P" "icall"
            build "$P" "enforcestatic"
        done
        ;;
    "musl")
        echo "Mode = $MODE"
        echo "$(date) | --- Start building with mode = $MODE ---" >> $LOGFILE
        for P in $PROGRAMS; do
            build "$P" "musl_enforce"
            build "$P" "musl_enforce_static"
            build "$P" "musl_instrument"
        done
        ;;
    "enforce" | "enforcestatic" | "icall" | "instrument" | "ref" | "musl_enforce" | "musl_enforce_static" | "musl_instrument")
        echo "Mode = $MODE"
        echo "$(date) | --- Start building with mode = $MODE ---" >> $LOGFILE
        for P in $PROGRAMS; do
            build "$P" $MODE
        done
        ;;
    *)
        echo "Invalid mode: $MODE"
        exit 1
        ;;
esac

