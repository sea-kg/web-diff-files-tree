# Build env stage
FROM debian:11

RUN apt-get update && apt-get install -y \
    cmake gcc g++ make pkg-config \
    default-libmysqlclient-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /opt/src
WORKDIR /opt/src
RUN mkdir -p tmp

WORKDIR /opt/src/tmp
RUN cmake ..
RUN make

# release stage
FROM debian:11

LABEL "maintainer"="Evgenii Sopov <mrseakg@gmail.com>"
LABEL "repository"="https://github.com/sea-kg/web-diff-files-tree"

RUN apt-get update && apt-get install -y \
    libmariadb3 \
    && rm -rf /var/lib/apt/lists/*

COPY html /opt/html
COPY --from=0 /opt/src/web-diff-files-tree /usr/bin/web-diff-files-tree

WORKDIR /opt/

CMD "web-diff-files-tree"
