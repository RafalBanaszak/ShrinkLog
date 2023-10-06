FROM gcc:13.2

RUN set -ex; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
		cmake \
		gdb \
        ninja-build \
        libhyperscan-dev \
        libxxhash-dev \
        libfmt-dev \
	;

#RUN cd; \
#    git clone https://github.com/Microsoft/vcpkg.git; \
#    cd vcpkg; \
#    ./bootstrap-vcpkg.sh; \
#    ./vcpkg integrate install; \
#    ./vcpkg install xxhash; \

# Set user and group
ARG user=appuser
ARG group=appuser
ARG uid=1000
ARG gid=1000
RUN groupadd -g ${gid} ${group}
RUN useradd -u ${uid} -g ${group} -s /bin/sh -m ${user}

# Switch to user
USER ${uid}:${gid}