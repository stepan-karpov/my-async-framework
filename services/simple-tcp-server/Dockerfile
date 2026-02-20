FROM ubuntu:24.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
 && rm -rf /var/lib/apt/lists/*

COPY my-async-framework /app/my-async-framework
COPY services/simple-tcp-server /app/services/simple-tcp-server

WORKDIR /app

# Build and install framework
RUN cd /app/my-async-framework && \
    cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DLOG_LEVEL=0 && \
    cmake --build build && \
    cmake --install build --prefix /app/local/my-async-framework

# Build service
RUN cd /app/services/simple-tcp-server && \
    cmake -S . -B build \
      -DCMAKE_PREFIX_PATH=/app/local/my-async-framework \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DLOG_LEVEL=0 && \
    cmake --build build

WORKDIR /app/services/simple-tcp-server

CMD ["bash"]