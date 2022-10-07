FROM ubuntu:20.04

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

# dependences for typro & souffle
RUN DEBIAN_FRONTEND=noninteractive \
    apt-get update && \
    apt-get -y upgrade && \
    apt-get install -y \
        cmake \
        build-essential \
        g++ \
        git \
        python3 \
        python-is-python3 \
        libz3-dev \
        libzip-dev \
        libtinfo-dev \
        libncurses-dev \
        libxml++2.6-dev \
        libsqlite3-dev \
        mcpp \
        apt-utils \
        wget \
        libgmp-dev \
        libmpfr-dev \
        p7zip-full \
        software-properties-common \
        libmpc-dev \
         && \
    apt-get clean

# Souffle has no official repo, we use their Github releases instead
RUN wget -O/tmp/souffle.deb 'https://github.com/souffle-lang/souffle/releases/download/2.3/x86_64-ubuntu-2004-souffle-2.3-Linux.deb' && \
    dpkg -i /tmp/souffle.deb && \
    rm /tmp/souffle.deb && \
    echo 'deb [ arch=amd64 ] https://downloads.skewed.de/apt focal main' > /etc/apt/sources.list.d/python-graph-tool.list && \
    apt-key adv --keyserver keyserver.ubuntu.com --recv-key 612DEFB798507F25 && \
    apt-get update && \
    apt-get install -y python3-graph-tool && \
    apt-get clean

# packages needed for tests (and some setup for the FTP servers)
RUN apt-key adv --keyserver keys.openpgp.org --recv-key 612DEFB798507F25 && \
    add-apt-repository "deb [ arch=amd64 ] https://downloads.skewed.de/apt focal main" && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get update && \
    apt-get install -y \
        python3-pip \
        python3-graph-tool \
        libcairo2-dev \
        libjpeg-dev \
        libgif-dev \
        libpam-dev \
        libcap-dev \
        automake \
        autoconf \
        tcl-dev \
        curl \
        sudo htop strace \
        qemu-user qemu-user-static binfmt-support \
        && \
    apt-get clean && \
    pip3 install pycairo numpy pwntools && \
    python3 -m pip install -U matplotlib && \
    mkdir -p /typro/scripts /typro/sysroots && \
    useradd -d /var/ftp -m -r -s /usr/sbin/nologin ftp && \
    echo Testfile-ABC > /var/ftp/test.txt && \
    chown root:root /var/ftp/test.txt && \
    mkdir -p /var/ftp/a/b/c/d/e/f && \
    echo FTP-Testfile > /var/ftp/a/b/c/d/e/f/test1.txt && \
    mkdir -p /var/ftp/tmp && \
    mkdir -p /var/ftp/var/ftp && \
    chmod 0777 /var/ftp/tmp/ && \
    ln -s /tmp /var/ftp/var/ftp/tmp && \
    useradd --create-home testuser && \
    echo 'testuser:testuser' | chpasswd

COPY ./llvm-typro /typro/llvm-typro
COPY ./scripts/build-setup.sh /typro/scripts/build-setup.sh
COPY ./scripts/cfi_blacklist.txt /typro/scripts/cfi_blacklist.txt

# build typro
WORKDIR /typro
RUN rm -rf /typro/build && \
    scripts/build-setup.sh && \
    cd build && \
    make -j $(nproc) clang lld llvm-typegraph typro-instrumentation typro-rt llvm-config llvm-ar llvm-ranlib llvm-dis && \
    ln -s /typro/build/lib /usr/typro-lib && \
    mkdir -p /typro/build/lib/clang/10.0.0/share/ && \
    cp /typro/scripts/cfi_blacklist.txt /typro/build/lib/clang/10.0.0/share/

COPY ./scripts /typro/scripts

# build musl libc
RUN apt-get update && \
    apt-get install -y clang-10 lld-10 && \
    apt-get clean && \
    mkdir -p /typro/sysroots && \
    ln -s /usr/bin/ld.lld-10 /usr/local/bin/ld.lld && \
    /typro/scripts/build-sysroots.sh && \
    /typro/scripts/build-runtime-libs.sh && \
    /typro/scripts/build-libraries-rt.sh && \
    /typro/scripts/build-libraries.sh && \
    rm -rf /typro/sysroots/*.src.tar.xz /typro/sysroots/musl*.tar.gz /typro/sysroots/*-work && \
    apt-get remove -y clang-10 lld-10 && \
    apt-get autoremove -y && \
    rm -f /usr/local/bin/ld.lld && \
    rm -rf /tmp/*

COPY ./examples /typro/examples
COPY ./tests /typro/tests

ENV PATH="/typro/build/bin:$PATH"
