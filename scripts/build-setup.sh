#!/usr/bin/env bash

# Prepares the "build" folder

set -e

cd "${0%/*}"
cd ..



mkdir -p build
cd build
rm -rf *
cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DLLVM_TARGETS_TO_BUILD="X86;AArch64;ARM;Mips" -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_ENABLE_ASSERTIONS=On ../llvm-typro/
echo ""
echo ""
echo ""
echo "Build directory prepared. To build run:"
echo "    cd `pwd`"
echo "    make -j `nproc` clang lld llvm-typegraph typro-instrumentation typro-rt"
