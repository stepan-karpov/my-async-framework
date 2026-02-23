FROM my-async-framework:latest

COPY services/simple-tcp-server /app/services/simple-tcp-server

WORKDIR /app

# Build service
RUN cd /app/services/simple-tcp-server && \
    cmake -S . -B build \
      -DCMAKE_PREFIX_PATH=/app/local/my-async-framework \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DLOG_LEVEL=0 && \
    cmake --build build

WORKDIR /app/services/simple-tcp-server

CMD ["/app/services/simple-tcp-server/build/SimpleTCPServer"]