FROM gcc:13.2

RUN set -ex; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
        libc6-dbg \
		cmake \
		gdb \
        ninja-build \
        libhyperscan-dev \
        libxxhash-dev \
        libfmt-dev \
        libyaml-cpp-dev \
	;

RUN cd; \
    git clone https://github.com/p-ranav/argparse.git; \
    cd argparse; \
    git checkout 69dabd88a8e6680b1a1a18397eb3e165e4019ce6; \
    mkdir build; \
    cd build; \
    cmake -DARGPARSE_BUILD_SAMPLES=off -DARGPARSE_BUILD_TESTS=off ..; \
    make; \
    make install;

#RUN apt-get -y install linux-perf

# Set user and group
ARG user=appuser
ARG group=appuser
ARG uid=1000
ARG gid=1000
RUN groupadd -g ${gid} ${group}
RUN useradd -u ${uid} -g ${group} -s /bin/sh -m ${user}

# Switch to user
USER ${uid}:${gid}