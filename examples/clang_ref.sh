#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
CLANG_ROOT=/typro/build
# alternative locations of clang
if [ ! -d "$CLANG_ROOT" ]; then
    CLANG_ROOT="$SCRIPT_DIR/../llvm-typro/cmake-build-minsizerel"
fi
if [ ! -d "$CLANG_ROOT" ]; then
    CLANG_ROOT="$SCRIPT_DIR/../build"
fi

exec "$SCRIPT_DIR/settings_ref.sh" "$CLANG_ROOT/bin/clang" "$@"
