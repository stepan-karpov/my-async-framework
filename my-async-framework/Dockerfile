# Base image for services using MyAsyncFramework.
# Build from repo root: yamo framework update 

FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

COPY my-async-framework /app/my-async-framework

WORKDIR /app/my-async-framework

RUN cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DLOG_LEVEL=0 && \
    cmake --build build && \
    cmake --install build --prefix /usr/local && \
    cd / && rm -rf /app/my-async-framework

WORKDIR /app