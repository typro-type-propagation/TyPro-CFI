
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd $SCRIPT_DIR
mkdir -p logs

MODE="${1:-enforce}"  # MODE: enforce, instrument, icall, ref
# this wrapper sets all TG_ settings (also for recompilations)
case "$MODE" in
    "enforce" | "enforcestatic" | "icall" | "instrument" | "ref" | "musl_enforce" | "musl_enforce_static" | "musl_instrument" | "arm64_ref" | "arm64_enforcestatic" )
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

if [ "$MODE" = "musl_enforce" ] || [ "$MODE" = "musl_enforce_static" ] || [ "$MODE" = "musl_instrument" ]; then
    MUSL_ROOT="$(dirname $SCRIPT_DIR)/sysroots/x86_64-linux-musl"
    export CC="$WRAPPER $MUSL_ROOT/bin/my-clang"
    export CXX="$WRAPPER $MUSL_ROOT/bin/my-clang++"
fi
if [ "$MODE" = "musl_enforce_static" ] || [ "$MODE" = "musl_instrument" ]; then
    export LDFLAGS="$LDFLAGS -static"
fi

# arm64
case "$MODE" in
    "arm64_ref" | "arm64_enforcestatic" )
        # flags for the apple M1 chip
        SYSROOT="$(dirname $SCRIPT_DIR)/sysroots/aarch64-linux-gnu"
        TARGET="aarch64-linux-gnu"
        GCC_VERSION="10"
        export TARGET_FLAGS="--target=aarch64-linux-gnu -Xclang -target-feature -Xclang +fp16fml -Xclang -target-feature -Xclang +fullfp16 -Xclang -target-feature -Xclang +sha3 -Xclang -target-feature -Xclang +v8.5a -Xclang -target-feature -Xclang +neon -Xclang -target-feature -Xclang +fp-armv8 -Xclang -target-feature -Xclang +crypto"
        export CFLAGS="$CFLAGS $TARGET_FLAGS --sysroot $SYSROOT \
            -isystem $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/include \
            -isystem $SYSROOT/usr/include/$TARGET \
            -isystem $SYSROOT/usr/include"
        export CXXFLAGS="$CXXFLAGS $TARGET_FLAGS --sysroot $SYSROOT \
            -isystem $SYSROOT/usr/include/c++/$GCC_VERSION \
            -isystem $SYSROOT/usr/include/$TARGET/c++/$GCC_VERSION \
            -isystem $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION/include \
            -isystem $SYSROOT/usr/include/$TARGET \
            -isystem $SYSROOT/usr/include"
        export LDFLAGS="$LDFLAGS $TARGET_FLAGS --sysroot $SYSROOT \
            -L$SYSROOT/usr/lib/$TARGET -L$SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION \
            -B $SYSROOT/usr/lib/$TARGET -B $SYSROOT/usr/lib/gcc/$TARGET/$GCC_VERSION"
        CONFIGURE_FLAGS="--host aarch64-linux-gnu"
        ;;
esac
