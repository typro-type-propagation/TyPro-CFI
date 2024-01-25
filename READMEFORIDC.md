TyPro with Indirectcall information: The "Ryan" branch is the  updated code
=============================================================================

what we have done:
1. get the basic block address that contains the indirect call
2. write the basic block address into a custom section in the original binary
3. get the basic block address and write it into file after complation
4. Add hash informtion of the indirect call in the final json file as a new object "tg_target_hash"


## 1. Pull the "Ryan" branch from the github
## 2. Compile
`cd ./build && make -j$(nproc) clang lld llvm-typegraph typro-instrumentation typro-rt`
## 3. Export settting variable
1. `export CC=/home/Typro/TyPro-CFI/build/bin/clang`
2. `export CXX=/home/Typro/TyPro-CFI/build/bin/clang++`
3. `export TG_CFI_OUTPUT=auto`
4. `export TG_ICFI_OUTPUT=auto`
5. `export TG_ENFORCE=false`
## 4. Test
1. `cd test-indirectCall`
2. `$CC test.c`
3. `Python3 getAddress.py`
4. the content of the address.txt is the address of indirect call