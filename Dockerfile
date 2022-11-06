FROM docker.io/library/ubuntu:20.04

LABEL maintainer="Marco Morais <marcoemorais@yahoo.com>"

# Base system including toolchain and dependencies.
RUN apt-get update && apt-get upgrade -y && DEBIAN_FRONTEND="noninteractive" apt-get install -y \
    build-essential \
    doctest-dev \
    less \
    tree \
    valgrind

# Use the gcc toolchain by setting CC and CXX environment variables.
ENV CC=/usr/bin/gcc
ENV CXX=/usr/bin/g++

## By convention, all development in /src.
WORKDIR /src
