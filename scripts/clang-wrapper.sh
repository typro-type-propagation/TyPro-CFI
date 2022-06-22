#!/bin/sh

set -eu

export TG_PROTECTED_LIBC=1

cc="CLANG_BIN_PATH/clang"
cxx="CLANG_BIN_PATH/clang++"
basedir="`cd "$(dirname "$0")/.."; pwd`"
case "$0" in
    *++) is_cxx=1 ;;
    *)   is_cxx= ;;
esac


# prevent clang from running the linker (and erroring) on no input.
sflags=
eflags=
is_linking=1
cleared=
for x ; do
    test "$cleared" || set -- ; cleared=1
    case "$x" in
        -l*)
            input=1
            sflags="-l-user-start"
            eflags="-l-user-end"
            set -- "$@" "$x"
            ;;
        -c)
            is_linking=
            set -- "$@" "$x"
            ;;
        -fuse-ld=*)
            ;;
        *)
            set -- "$@" "$x"
    esac
done


# C++ params
clear_include="-nostdinc"
additional_includes=
if test "$is_cxx"; then
    cc=$cxx
    clear_include="$clear_include -nostdinc++"
    additional_includes="-isystem $basedir/include/c++/v1"
fi

# Linking params
if test "$is_linking"; then
    linker_flags1="-fuse-ld=my-clang -static-libgcc"
    linker_flags2="-L$basedir/lib -L-user-end"
    sflags="-L-user-start $sflags"
    clear_include=
else
    linker_flags1=
    linker_flags2=
fi

# Linking C++ params
if test "$is_cxx"; then
    if test "$is_linking"; then
        linker_flags1="$linker_flags1 -stdlib=libc++"
    fi
fi



# echo exec $cc \
#     -B"$basedir/bin" \
#     $linker_flags1 \
#     $clear_include \
#     --sysroot "$basedir" \
#     $additional_includes \
#     -isystem "$basedir/include" \
#     -isystem "$basedir/usr/include" \
#     $sflags \
#     "$@" \
#     $eflags \
#     $linker_flags2

exec $cc \
    -B"$basedir/bin" \
    $linker_flags1 \
    $clear_include \
    --sysroot "$basedir" \
    $additional_includes \
    -isystem "$basedir/include" \
    -isystem "$basedir/usr/include" \
    -isystem "$basedir/usr/lib/clang/10/include" \
    $sflags \
    "$@" \
    $eflags \
    $linker_flags2

