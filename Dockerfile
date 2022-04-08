FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y make cpio libncurses5 wget curl xz-utils git cmake doxygen

WORKDIR /home

RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -O gcc-arm-none-eabi.tar.bz2
RUN mkdir gcc-arm-none-eabi && tar xjfv gcc-arm-none-eabi.tar.bz2 -C gcc-arm-none-eabi --strip-components 1
RUN rm gcc-arm-none-eabi.tar.bz2
ENV PATH="/home/gcc-arm-none-eabi/bin:${PATH}"

RUN curl -SL https://github.com/llvm/llvm-project/releases/download/llvmorg-14.0.0/clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz \
    | tar -xJC . \
    && mv clang+llvm-14.0.0-x86_64-linux-gnu-ubuntu-18.04 clang
ENV PATH="/home/clang/bin:${PATH}"

WORKDIR /home/OBC2