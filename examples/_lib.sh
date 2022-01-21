
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd $SCRIPT_DIR
mkdir -p logs

MODE="${1:-enforce}"  # MODE: enforce, instrument, icall, ref
# this wrapper sets all TG_ settings (also for recompilations)
case "$MODE" in
    "enforce" | "enforcestatic" | "icall" | "instrument" | "ref")
        echo "Mode = $MODE"
        ;;
    *)
        echo "Invalid mode: $MODE"
        exit 1
        ;;
esac
WRAPPER="$SCRIPT_DIR/settings_$MODE.sh"
CC2="$SCRIPT_DIR/clang_$MODE.sh"

CLANG_ROOT=/typro/build
# alternative locations of clang
if [ ! -d "$CLANG_ROOT" ]; then
    CLANG_ROOT="$SCRIPT_DIR/../llvm-typro/cmake-build-minsizerel"
fi
if [ ! -d "$CLANG_ROOT" ]; then
    CLANG_ROOT="$SCRIPT_DIR/../build"
fi

INSTALL_DIR="$SCRIPT_DIR/install_$MODE"
mkdir -p "$INSTALL_DIR"

# Define default flags
export CC="$WRAPPER $CLANG_ROOT/bin/clang"
export CXX="$WRAPPER $CLANG_ROOT/bin/clang++"
if [ "$MODE" = "icall" ]; then
    export CFLAGS="-O3 -flto -fsanitize=cfi-icall -fvisibility=default"
    export CXXFLAGS="-O3 -flto -fsanitize=cfi-icall -fvisibility=default"
    export LDFLAGS="-O3 -flto -fsanitize=cfi-icall -fvisibility=default -L$INSTALL_DIR/lib -Wl,-rpath,$INSTALL_DIR/lib"
else
    export CFLAGS="-O3 -flto"
    export CXXFLAGS="-O3 -flto"
    export LDFLAGS="-O3 -flto -L$INSTALL_DIR/lib -Wl,-rpath,$INSTALL_DIR/lib"
fi
