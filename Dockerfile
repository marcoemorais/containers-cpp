# Standard ubuntu desktop operating system.
FROM ubuntu:20.04

LABEL maintainer="Marco Morais <marcoemorais@yahoo.com>"

# Base system including toolchain and dependencies.
RUN apt-get update && apt-get upgrade -y && DEBIAN_FRONTEND="noninteractive" apt-get install -y \
    build-essential \
    doctest-dev \
    git \
    less \
    libspdlog-dev \
    tree \
    valgrind

# Explode manpages.
RUN yes | unminimize && \
    apt-get install -y man-db && \
    rm -r /var/lib/apt/lists/*

# Use the gcc toolchain by setting CC and CXX environment variables.
ENV CC=/usr/bin/gcc
ENV CXX=/usr/bin/g++

# Installation prefix for third party libraries.
ARG PREFIX_DIRECTORY=/usr/local

# C++ reference.
ARG STDMAN_TAG=2020.11.17
RUN git clone -q https://github.com/jeaye/stdman.git /stdman-${STDMAN_TAG} --branch ${STDMAN_TAG} \
    && mkdir -p /stdman-${STDMAN_TAG} \
    && cd /stdman-${STDMAN_TAG} \
    && ./configure --prefix=${PREFIX_DIRECTORY} \
    && make install \
    && rm -rf /stdman-${STDMAN_TAG}

## By convention, all development in /src.
WORKDIR /src
