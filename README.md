TyPro: Forward CFI for C-Style Indirect Function Calls Using Type Propagation
=============================================================================
TyPro is a C compiler which protects C-style indirect calls from being tampered.
The relevant publication is currently under submission.
TyPro is a fork of LLVM 10, including Clang and lld.
It can be used as a drop-in replacement for gcc or clang in most situations.

## Installation
We tested TyPro on Ubuntu 20.04 and Debian 11. Through it will likely work on other architectures, we can't guarantee that.

### Docker
To simplify setup, TyPro can be used in a self-contained Docker container.
Compiler lies in `/typro/build/bin/clang`.

- Build an image: `docker build -t typro .`
- Invoke compiler in temporary container: `docker run --rm typro clang --version`
- Create permanent container: `docker run --name typro -v "/:/mnt" -it typro`
- Restart permanent container: `docker start -i typro`
- Get second shell in permanent container: `docker exec -it typro bash`

### Native
1. Install dependencies: `apt install -y cmake build-essential g++ git python3 libz3-dev libzip-dev libtinfo-dev libncurses-dev libxml++2.6-dev libsqlite3-dev mcpp apt-utils wget libgmp-dev libmpfr-dev libmpc-dev`
2. Install the Souffle Datalog Engine from [Github](https://github.com/souffle-lang/souffle/releases)
3. Clone this repository
4. Setup a build directory: `scripts/build-setup.sh`
5. Build: `cd ./build && make -j$(nproc) clang lld llvm-typegraph typro-instrumentation typro-rt`
6. Create a symlink for runtime libs: `sudo ln -s $(pwd)/build/lib /usr/typro-lib`


## How To Use
TyPro can be used like any other compiler. 
In most build systems (like Automake and CMake), you just have to set the environment variable `CC`:

- Docker: `export CC=clang`
- Native: `export CC=/<typro>/build/bin/clang`

TyPro defaults to its lld linker and has link time optimizations enabled by default.
Libraries are compiled with dynamic linking in mind, programs not (except `TG_DYNLIB_SUPPORT` is set, see below).


## Configuration
TyPro can be configured by environment variables, both in Clang and lld.
Default setting is: extract facts ("graph"), enforce target sets, no dynamic libraries.
Additional options help to fine-tune the protection for your applications, to debug the generated facts and target sets, and to export necessary evaluation data.

| _Setting_                          | _Default_ | _Description_                                                                                                                  |
|------------------------------------|-----------|--------------------------------------------------------------------------------------------------------------------------------|
| `TG_ENABLED`                       | true      | Generate facts and target sets                                                                                                 |
| `TG_ENFORCE`                       | true      | Enforce target sets                                                                                                            |
| `TG_DYNLIB_SUPPORT`                | false     | Build trampolines and module summaries (for linking with libraries)                                                            |
| `TG_ENFORCE_ID_BITWIDTH`           | 31        | Max size of function IDs (up to 64). Smaller IDs have higher chance of collision, but generate faster code.                    |
| `TG_ENFORCE_MIN_ID`                | 2         | Smallest ID any function can have. If you use constants casted to function pointers (like SIG_IGN), this value must be higher. |
| `TG_ENFORCE_DEBUG`                 | false     | Generate debug code for target set enforcement (see which function might violate generated target sets)                        |
| `TG_CLANG_SHOW_GRAPH`              | false     | Show the (interesting part of the) generated facts of a source file                                                            |
| `TG_CLANG_MINIMIZE_GRAPH`          | true      | Optimize generated facts for each source file                                                                                  |
| `TG_INSTRUMENT_COLLECTCALLTARGETS` | false     | Instrument indirect calls to collect targets during program execution                                                          |
| `TG_GRAPH_OUTPUT`                  | -         | Path to write the collected facts to (can be `auto` for source.o.typegraph)                                                    |
| `TG_CFI_OUTPUT`                    | -         | Path to write the generated target sets in JSON format (can be `auto` for binary.tgcfi.json)                                   |
| `TG_ICFI_OUTPUT`                   | -         | Path to write Clang CFI's target sets in JSON format (can be `auto`)                                                           |
| `TG_IFCC_OUTPUT`                   | -         | Path to write IFCC's target sets in JSON format (can be `auto`)                                                                |


## Running tests
We provide 150+ unit tests you can use to check the compiler and your setup.

**Docker**: `docker run --rm --workdir /typro/tests typro python3 -u -m unittest typegraph_test.py`

**Native**: Install [python3-graph-tool](https://graph-tool.skewed.de/). Then run `python3 -u -m unittest typegraph_test.py` in the folder `tests`.


## Testing with SPEC CPU 2006
TyPro has been evaluated on programs from the SPEC CPU 2006 benchmark. 
We tested all programs that are pure C, and that use indirect calls.
These programs are: bzip2, perlbench, gcc, milc, gobmk, hmmer, sjeng, h264ref, sphinx3.
You need a copy of [SPEC CPU 2006](https://www.spec.org/cpu2006/). 
We install from an iso image, adapt if you have a physical disk.

Copy iso to container: `docker cp /.../spec-cpu2006.iso typro:/tmp/spec-cpu2006.iso`

In docker, install SPEC:
```
mkdir -p /tmp/spec2006
cd /tmp/spec2006
7z x ../spec-cpu2006.iso
find ./tools/ -type f -exec chmod +x {} \;
find ./bin/ -type f -exec chmod +x {} \;
find ./ -type f -iname "*.sh" -exec chmod +x {} \;
mkdir -p /spec
./install.sh -d /spec
```

Configure SPEC in docker: 
```
cd /spec
ln -s /typro/build/ typro_build
cp /typro/scripts/*.cfg config/
```

Run SPEC builds / benchmarks in docker:
```
# enter SPEC runtime
cd /spec
. ./shrc
# build one
runspec -a build -c spec-clang-lto-o3-typro-shielded.cfg bzip2
# build all
runspec -a build -c spec-clang-lto-o3-typro-shielded.cfg bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
# run all (test input)
runspec -c spec-clang-lto-o3-typro-shielded.cfg --noreportable -i test -n 1 bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
```

In a **native** environment, use the configuration file from the `scripts` directory. 
Create a symlink named `typro_build` in SPEC's root, pointing to a build directory of TyPro.


## Testing example programs
We evaluated TyPro on 7 real-world programs: Lighttpd, Apache Httpd, Memcached, Nginx, PureFTPd, Redis, vsFTPd.

You can compile protected versions of all example programs (bring some time): `cd /typro/examples && ./build_all.sh`

Log files are created in `/typro/logs`. At the end of each `build-all-*-enforce.log` file, you find instructions how a program's functionality can be tested. The necessary configuration files are provided.


## Cross-compiling to Architectures
TyPro can cross-compile programs for AArch64 (64-bit ARM) and MIPS64el (64bit little endian MIPS), we provide scripts to set up a build environment.
To this end, you will need a "sysroot", which is a partial image of a minimal linux for the target architecture.
It contains all libraries and include files necessary to compile against this system, and test programs using _qemu_.
Setup is similar in docker and native, except the path of the scripts.

The sysroot can't be created in a docker container for lack of permissions. 
Therefore you'll have to build sysroots on your native system.
After initial creation, sysroots are archived in .tar.xz files in `sysroots/` and can be re-used in the docker container or on other systems.
To do so, simply extract the archives in the sysroots folder.

On your native system:
- Install dependencies: `apt install -y binfmt-support qemu qemu-user qemu-user-static debootstrap`
- Build sysroots: `sudo ./scripts/crosscompile-create-sysroots.sh`

Install sysroots in docker (skip for native setup):
- Install dependencies: `apt install -y binfmt-support qemu qemu-user qemu-user-static` and `apt install --no-install-recommends clang-10`
- Copy sysroots to docker (run command on host): `for f in sysroots/sysroot-*.tar.xz; do docker cp $f typro:/tmp/; done`
- Install sysroots:
  ```
  mkdir -p /typro/sysroots
  cd /typro/sysroots
  for f in /tmp/sysroot-*.tar.xz; do tar -xf $f; done
  ```

Setup cross-compilation (adjust paths for native setups):
- Test sysroot and qemu:
  - `qemu-aarch64 -L /typro/sysroots/aarch64-linux-gnu/ /typro/sysroots/aarch64-linux-gnu/usr/bin/id`
  - `qemu-mips64el -L /typro/sysroots/mips64el-linux-gnuabi64/ /typro/sysroots/mips64el-linux-gnuabi64/usr/bin/id`
- Build and install runtime libraries: `scripts/crosscompile-prepare-symlinks.sh && scripts/crosscompile-runtime-libs.sh` (if you don't have `clang-10` available, patch the script to another existing cross-compiler)
- Run tests against cross-compiler:
  - `cd tests && ARCH=aarch64 python3 -u -m unittest typegraph_test.py`
  - `cd tests && ARCH=mips64el python3 -u -m unittest typegraph_test.py`


## Interesting Code
- [llvm-typro/tools/clang/lib/CodeGen/TypegraphBuilder.h](llvm-typro/tools/clang/lib/CodeGen/TypegraphBuilder.h)
- [llvm-typro/tools/clang/lib/CodeGen/TypegraphBuilder.cpp](llvm-typro/tools/clang/lib/CodeGen/TypegraphBuilder.cpp)
- [llvm-typro/include/llvm/Typegraph](llvm-typro/include/llvm/Typegraph)
- [llvm-typro/lib/Typegraph](llvm-typro/lib/Typegraph)
- [llvm-typro/lib/TypegraphPasses](llvm-typro/lib/TypegraphPasses)


## Acknowledgements
This project is based on (and contains a modified copy of) [LLVM](https://llvm.org/), [Clang](https://clang.llvm.org/) and [lld](https://lld.llvm.org/), version 10, taken from [the official releases](https://releases.llvm.org/). 

