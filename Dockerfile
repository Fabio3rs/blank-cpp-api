FROM ubuntu:20.04 as build

LABEL description="Build container"

RUN apt update #
RUN apt install -y clang-12 clang-tidy-12 clang-format-12 ninja-build cmake clang-12
RUN apt install libcurl4 libcurl4-openssl-dev libpoco-dev libmysqlcppconn-dev libgtest-dev googletest redis-server redis-tools -y
RUN apt install mariadb-client libmysqlcppconn-dev -y
RUN apt install pkg-config zip unzip -y
RUN apt install clang-format git -y

ENV CC=/usr/bin/clang-12
ENV CXX=/usr/bin/clang++-12

RUN mkdir /src
ADD . /src
RUN mkdir -p /src/build

RUN rm /src/build/CMakeCache.txt || echo "CMakeCache do not exists"
RUN cd /src/build && cmake .. -G Ninja && cmake --build . --config Debug --target all -j $(nproc) --

RUN cd /src/build/bin && /src/packager blank_cpp_api && ls -lahtr built

FROM alpine:latest AS runtime

RUN apk --no-cache add ca-certificates
ENV CUSTOM_CA_BUNDLE=/etc/ssl/cert.pem

ENV LAMBDA_TASK_ROOT=/root
WORKDIR /root/
RUN mkdir api
COPY --from=0 /src/build/bin/built/ ./

CMD ["/root/bootstrap"]

