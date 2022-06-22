FROM ubuntu:20.04

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

# dependences for souffle
# RUN DEBIAN_FRONTEND=noninteractive \
#     apt-get update && \
#     apt-get -y upgrade && \
#     apt-get install -y \
#         autoconf \
#         automake \
#         cmake \
#         bison \
#         build-essential \
#         clang \
#         doxygen \
#         flex \
#         g++ \
#         git \
#         libffi-dev \
#         libncurses5-dev \
#         libtool \
#         libsqlite3-dev \
#         make \
#         mcpp \
#         python \
#         sqlite \
#         zlib1g-dev \
#         apt-utils && \
#     apt-get clean

# # build && install souffle
# RUN git clone https://github.com/souffle-lang/souffle.git && \
#     cd /souffle && \
#     git checkout d9209ddda1b831659494f015638956dfb618ac98 && \
#     sh ./bootstrap && \
#     ./configure && \
#     make -j $(nproc) && \
#     make install && \
#     cd / && \
#     rm -rf /souffle
#ENV PATH="/souffle-bin:${PATH}"

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
        libmpc-dev && \
    apt-get clean

# Souffle has no official repo, we use their Github releases instead
RUN wget -O/tmp/souffle.deb 'https://github.com/souffle-lang/souffle/releases/download/2.3/x86_64-ubuntu-2004-souffle-2.3-Linux.deb' && \
    dpkg -i /tmp/souffle.deb && \
    rm /tmp/souffle.deb

# packages needed for tests
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
        sudo && \
    apt-get clean
RUN pip3 install pycairo numpy
RUN python3 -m pip install -U matplotlib

COPY . /typro

# build typro
WORKDIR /typro
RUN rm -rf /typro/build && \
    scripts/build-setup.sh && \
    cd build && \
    make -j $(nproc) clang lld llvm-typegraph typro-instrumentation typro-rt llvm-config llvm-ar llvm-ranlib llvm-dis && \
    ln -s /typro/build/lib /usr/typro-lib && \
    mkdir -p /typro/build/lib/clang/10.0.0/share/ && \
    cp /typro/scripts/cfi_blacklist.txt /typro/build/lib/clang/10.0.0/share/

# build musl libc
RUN apt-get update && \
    apt-get install -y clang-10 lld-10 && \
    apt-get clean && \
    mkdir /typro/sysroots && \
    ln -s /usr/bin/ld.lld-10 /usr/local/bin/ld.lld && \
    /typro/scripts/build-libraries-rt.sh && \
    /typro/scripts/build-libraries.sh && \
    rm -rf /typro/sysroots/*.src.tar.xz /typro/sysroots/musl*.tar.gz /typro/sysroots/*-work && \
    apt-get remove -y clang-10 lld-10 && \
    apt-get autoremove -y && \
    rm -f /usr/local/bin/ld.lld

ENV PATH="/typro/build/bin:$PATH"
