FROM ubuntu:24.04

RUN apt-get update && apt-get install -y curl build-essential git python3 && \
    BAZEL_VERSION=8.4.1 && \
    ARCH=$(dpkg --print-architecture) && \
    if [ "$ARCH" = "arm64" ]; then BAZEL_ARCH="linux-arm64"; else BAZEL_ARCH="linux-x86_64"; fi && \
    curl -fLO "https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/bazel-${BAZEL_VERSION}-${BAZEL_ARCH}" && \
    chmod +x "bazel-${BAZEL_VERSION}-${BAZEL_ARCH}" && \
    mv "bazel-${BAZEL_VERSION}-${BAZEL_ARCH}" /usr/local/bin/bazel && \
    rm -rf /var/lib/apt/lists/* \

WORKDIR /app
COPY . .

RUN bazel build //broker:broker //server:server //client:client

CMD ["./bazel-bin/broker/broker"]