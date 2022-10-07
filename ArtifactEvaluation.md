TyPro: Forward CFI for C-Style Indirect Function Calls Using Type Propagation
=============================================================================
Artifact Evaluation Guidance
----------------------------
TyPro is a C compiler which protects C-style indirect calls from being tampered.
The relevant publication is currently under submission.
TyPro is a fork of LLVM 10, including Clang and lld.
It can be used as a drop-in replacement for gcc or clang in most situations.

TyPro has been presented in our [ACSAC'22](https://www.acsac.org/2022/) paper ["TyPro: Forward CFI for C-Style Indirect Function Calls Using Type Propagation"](https://publications.cispa.saarland/3768/) by Markus Bauer, Ilya Grishchenko, and Christian Rossow.
For questions contact Markus Bauer <markus.bauer@cispa.saarland>.

In this evaluation guideline, we will verify that:
1. TyPro compiles successfully
2. TyPro can build simple programs (and the compiled programs work as expected) *(Section 6.1)*
3. TyPro can build the SPEC CPU 2006 benchmarks, they run successfully, and we evaluate its precision compared to IFCC and Clang CFI *(Section 6.1+6.2, in particular Table 1)* 
4. TyPro can build the real-world applications, they run successfully, and we evaluate its precision compared to IFCC and Clang CFI *(Section 6.1+6.2, in particular Table 2)*
5. TyPro can protect MUSL libc and can build fully protected programs based on MUSL libc *(Section 6.4)*

**Requirements:**
To reproduce our results, you need a 64-bit x86 Docker host with at least 16GB RAM and 25GB of disk space.
Also, a copy of SPEC CPU 2006 is required.
Serial compilation and execution of all programs takes around 9h on a modern system.
Running different commands in parallel is often possible (for example: compiling different examples in parallel) given enough RAM, use multiple shells for that.
The expected runtimes were gathered on an i7 9700K workstation with 64GB memory and an NVME SSD disk.

## Installation
We tested TyPro on Ubuntu 20.04 and Debian 11. Through it will likely work on other architectures, we can't guarantee that.
These instructions assume you use our provided Docker container to evaluate TyPro, which is based on Ubuntu 20.04.
If you want to install TyPro natively, follow the instructions from [README.md](README.md), in particular the installation and the section "Cross-compiling to Architectures".
In this case, you might need to adapt paths in all steps. Instructions assume TyPro in `/typro` and SPEC CPU 2006 in `/spec`.

## 1. Compile TyPro, compile runtime libraries and build sysroots for cross-compilation
*(~30min)*

The Dockerfile builds everything: TyPro, runtime libraries, sysroots for x86_64, aarch64, and MIPS64el, and a protected musl libc.
- Clone the repository: `git clone "https://github.com/typro-type-propagation/TyPro-CFI.git" typro ; cd typro`
- Build the image: `docker build -t typro .`
- Create permanent container: `docker run --name typro -v "/:/mnt" -it typro`
- (If necessary) get second shell in permanent container: `docker exec -it typro bash`

## 2. Running unit/integration tests
*(~5min)*

We provide 220+ unit tests you can use to check the compiler and your setup.

Open a shell in the container and run: 
```shell
cd /typro/tests
python3 -u -m unittest typegraph_test.py
ARCH=aarch64 python3 -u -m unittest typegraph_test.py
ARCH=mips64el python3 -u -m unittest typegraph_test.py
```

Verify that each call ends with `Ran 227 tests in 12.345s` and either `OK (skipped=1)` or `OK (skipped=30)`.



## 3. Testing with SPEC CPU 2006
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
./install.sh -f -d /spec
rm /tmp/spec-cpu2006.iso
```

Configure SPEC in docker: 
```
cd /spec
ln -s /typro/build/ typro_build
cp /typro/scripts/*.cfg config/
```


### Verify that TyPro can handle SPEC benchmarks
*(~50min)*

Run SPEC builds / benchmarks in docker. In theory, you can build/run different benchmarks in different shells to get more multiprocessing.
```
# enter SPEC runtime
cd /spec
. ./shrc
# build all
runspec -a build -c spec-clang-lto-o3-typro-shielded.cfg bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
# run all (test input)
runspec -c spec-clang-lto-o3-typro-shielded.cfg --noreportable -i test -n 1 bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
# run all (ref input)
runspec -c spec-clang-lto-o3-typro-shielded.cfg --noreportable -i ref -n 1 bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
```

When running the tests, you should see this line close to the end of the output (before the indented part): `Success: 1x400.perlbench 1x401.bzip2 1x403.gcc 1x433.milc 1x445.gobmk 1x456.hmmer 1x458.sjeng 1x464.h264ref 1x482.sphinx3`


### Collect target set information of TyPro, IFCC and ClangCFI
*(~120min, can be parallelized)*

First we build the benchmarks, protected with Clang CFI and export target sets.
Target set infos for Clang CFI will be in files named `/spec/benchspec/CPU2006/*/build/build_*clangcfi*/icfi_icall.json`.
```shell
runspec -a build -c spec-clang-lto-o3-typro-clangcfi.cfg bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
```

Next, we build the benchmarks with instrumentation and extract TyPro/IFCC target sets.
Target sets will be in files named `/spec/benchspec/CPU2006/*/build/build_*instrumented*/tgcfi.json` (TyPro) and `/spec/benchspec/CPU2006/*/build/build_*instrumented*/ifcc.json` (IFCC).
Furthermore, we want to collect information which indirect calls are used during execution and which functions they target.
We use this information later to validate that target sets are correct.
Runtime call targets are in files `/spec/benchspec/CPU2006/*/run/run_*instrumented*/calltargets*.json`.
We have a script that combines and summarizes all these files similar to Table 1 and Table 2 from the paper (see next section).
```shell
runspec -a build -c spec-clang-lto-o3-typro-instrumented.cfg bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
runspec -c spec-clang-lto-o3-typro-instrumented.cfg --noreportable -i test -n 1 bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
runspec -c spec-clang-lto-o3-typro-instrumented.cfg --noreportable -i train -n 1 bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
runspec -c spec-clang-lto-o3-typro-instrumented.cfg --noreportable -i ref -n 1 bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
```

### Validate correctness and precision of TyPro/IFCC/ClangCFI
The script `tests/precision_tool.py` parses all target sets and call targets, and visualizes them in a table form.
You can compare the produced table to Table 1 from the paper.

```shell
python3 /typro/tests/precision_tool.py /spec/benchspec/CPU2006/4*.*
```


## 4. Testing example programs
*(~5h, can be parallelized)*

We evaluated TyPro on 7 real-world programs: Lighttpd, Apache Httpd, Memcached, Nginx, PureFTPd, Redis, vsFTPd.

Again, we will build three versions: TyPro-protected, instrumented and Clang-CFI-protected. 
However, unlike SPEC, you will have to test each program manually.

You can compile all example programs (bring some time). Given enough RAM, you can run all scripts in parallel shells. 
```shell
cd /typro/examples
./build_all.sh enforcestatic
./build_all.sh instrument
./build_all.sh icall
./redis_docker.sh enforce  # redis must be built with dynamic linking support on
```

After compilation, you have a set of files for each program:
- `...-instrument/.../tgcfi.json` Target sets of TyPro
- `...-icall/.../icfi_icall.json` Target sets of Clang CFI
- `...-instrument/.../ifcc.json` Target sets of IFCC
- `...-instrument/.../calltargets*.json` Actually called functions for each binary


### Testing Apache Httpd
Start the protected server, then test it with the curl commands below (which you execute in a second shell within docker):
```shell
cd /typro/examples/httpd/httpd-2.4.51-enforcestatic
./httpd -DFOREGROUND
```

Test connections to the server (including expected output):
```shell
curl 'http://localhost:12346/'  # => <h1>Hello World!</h1>
curl 'http://localhost:12346/index.html'  # => <h1>Hello World!</h1>
curl 'http://localhost:12346/texts'  # => ... <title>301 Moved Permanently</title> ...
curl 'http://localhost:12346/texts/'  # => <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN"> ...
curl 'http://localhost:12346/texts/test.txt'  # => Hello World!
curl 'http://localhost:12346/cgi-bin/date'  # => <current date>
curl 'http://localhost:12346/cgi-bin/date.sh'  # => <current date>
curl 'http://localhost:12346/' --header 'HTTP_PROXY: 127.0.0.1'  # => <h1>Hello World!</h1>
```
Then terminate the server. 
Check the error log: `cat /typro/examples/install_enforcestatic/logs/apache_error.log`.
It should not contain any unexpected error (`resuming normal operations`, `Command line` or `caught SIGTERM, shutting down` are ok).

Next, execute the instrumented server and repeat the curl commands:
```shell
cd /typro/examples/httpd/httpd-2.4.51-instrument
./httpd -DFOREGROUND
```

### Testing lighttpd
Lighttpd has integration tests:
```shell
cd /typro/examples/lighttpd/lighttpd-1.4.59-enforcestatic/tests
make check
cd /typro/examples/lighttpd/lighttpd-1.4.59-instrument/tests
make check
```

### Testing Memcached
Memcached provides integration tests:
```shell
cd /typro/examples/memcached/memcached-1.6.10-enforcestatic/
make test
cd /typro/examples/memcached/memcached-1.6.10-instrument/
make test
```

After each `make test`, you should see a green line: `All tests successful.`

### Testing Nginx
Start the protected server, then test it with the curl commands below (which you execute in a second shell within docker):
```shell
cd /typro/examples/nginx/nginx-release-1.20.0-enforcestatic/objs/
./nginx
```

Test connections to the server (including expected parts of the output):
```shell
curl 'http://localhost:8000/' -o -  # => <h1>Hello World!</h1>
curl 'http://localhost:8000/texts/test.txt' -o -  # => Hello World!
curl 'http://localhost:8001/' -o -  # => <h1>Hello World!</h1>
curl 'http://localhost:8001/test.txt' -o -  # => Hello World!
curl 'http://localhost:8001/test.txt' -o - --compressed -v  # => < Content-Encoding: gzip [...] Hello World!
curl 'http://localhost:8001/test2.txt' -o - --compressed -v  # => <center><h1>404 Not Found</h1></center>
curl 'http://localhost:8000/try/abc' -o -  # => <center><h1>500 Internal Server Error</h1></center>
curl 'http://localhost:8000/shm' -o -  # => <center><h1>404 Not Found</h1></center>
curl 'http://localhost:8001/test.txt' -o - -r 0-2  # => Hel
curl 'http://localhost:8001/test.txt' -o - --http2  # => Hello World!
```
Then terminate the server, no errors should have appeared in its shell.

Next, execute the instrumented server and repeat the curl commands:
```shell
cd /typro/examples/nginx/nginx-release-1.20.0-instrument/objs/
TG_RT_CALLTARGET_BASE=/tmp/nginx ./nginx
```
After all curl commands have been executed, terminate the server with Ctrl+C.
Finally, copy the collected call targets back:
```shell
cp /tmp/nginx.calltargets*.json /typro/examples/nginx/nginx-release-1.20.0-instrument/objs/
```

### Testing PureFTPd
Start the FTP server:
```shell
cd /typro/examples/pureftpd/pureftpd-enforcestatic/src
./pure-ftpd ../../../pure-ftpd.conf
```

Test connections (and expected output):
```shell
curl ftp://localhost:2121/ -l  # => var\na\n...\ntmp\n...\ntest.txt
curl ftp://localhost:2121/  # => [...] -rw-r--r--    1 0          0                  29 Sep  7 21:12 test.txt [...]
curl ftp://localhost:2121/test.txt  # => Testfile-ABC
rm -f /var/ftp/ftp.test
curl -T /etc/passwd ftp://localhost:2121/ftp.test  # => <some statistics>
curl -T /etc/passwd ftp://localhost:2121/ftp.test  # => curl: (25) Failed FTP upload: 550
curl --user testuser:abc ftp://localhost:2121/  # => curl: (67) Access denied: 530
curl --user testuser:testuser ftp://localhost:2121/  # => drwxr-xr-x    2 testuser   testuser         4096 Sep  7 21:31 . [...]
python3 -u /typro/examples/pureftpd-tests.py  # => [OK]
```

Start the instrumented server, re-run the test commands above:
```shell
cd /typro/examples/pureftpd/pureftpd-instrument/src
TG_RT_CALLTARGET_BASE=/var/ftp/tmp/pure-ftpd ./pure-ftpd ../../../pure-ftpd.conf
```

Finally, copy the calltarget results back:
```shell
cp /var/ftp/tmp/pure-ftpd.calltargets* /typro/examples/pureftpd/pureftpd-instrument/src/
```

### Testing Redis
Redis provides integration tests. 
These tests also include dynamic loading: they check if Redis can load other (protected) modules at runtime.
```shell
cd /typro/examples/redis/redis-enforce/
./runtest
```
The test should finish with a line: `\o/ All tests passed without errors!`

Next, we run the tests on the instrumented server to collect call targets:
```shell
cd /typro/examples/redis/redis-instrument/
TG_RT_SILENT=1 TG_RT_CALLTARGET_BASE=$(pwd)/src/redis-server ./runtest
```

### Testing vsFTPd
Start the FTP server:
```shell
cd /typro/examples/vsftpd/vsftpd-3.0.5-enforcestatic/
./vsftpd ../../vsftpd.conf
```

Run these commands to test the server (comments show expected output):
```shell
curl -l ftp://localhost:2121  # => bin\nboot ... usr\nvar
curl ftp://localhost:2121/etc/passwd  # => root:x:0:0:root:/root:/bin/bash [...]
rm -f /tmp/ftp.test
curl -T /etc/passwd ftp://localhost:2121/tmp/ftp.test  # => <transfer statistics>
curl -T /etc/passwd ftp://localhost:2121/tmp/ftp.test  # => curl: (25) Failed FTP upload: 553
```
Terminate the server with Ctrl+C.

Next, execute the instrumented server and repeat the test commands:
```shell
cd /typro/examples/vsftpd/vsftpd-3.0.5-instrument/
./vsftpd ../../vsftpd.conf
```

### Validate correctness and precision of TyPro/IFCC/ClangCFI
You already verified that TyPro does not introduce errors into the protected applications.
The script `tests/precision_tool.py` parses all target sets and call targets from all examples, and visualizes them in a table form.
You can compare the produced table to Table 2 from the paper:

```
python3 /typro/tests/precision_tool.py /typro/examples/*
```


## MUSL libc
*(20min)*

We already ensured that musl builds (as part of the Dockerfile): `/typro/sysroots/x86_64-linux-musl/lib/libc.so`
We also showed that this musl libc works because musl was tested as part of the unit tests.
We now expand these tests by compiling the SPEC benchmarks against musl libc:
```shell
cd /spec
. ./shrc
runspec -c spec-clang-lto-o3-typro-musl.cfg --noreportable -i test -n 1 bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
runspec -c spec-clang-lto-o3-typro-musl-dynamic.cfg --noreportable -i test -n 1 bzip2 perlbench gcc milc gobmk hmmer sjeng h264ref sphinx3
```
