FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get install -y wget make cpio libncurses5 git cmake clang-tidy clang-format doxygen

WORKDIR /home

RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -O gcc-arm-none-eabi.tar.bz2
RUN mkdir gcc-arm-none-eabi && tar xjfv gcc-arm-none-eabi.tar.bz2 -C gcc-arm-none-eabi --strip-components 1
RUN rm gcc-arm-none-eabi.tar.bz2
ENV PATH="/home/gcc-arm-none-eabi/bin:${PATH}"

WORKDIR /home/OBC2