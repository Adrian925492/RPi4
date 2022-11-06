# Use latest ubuntu image as base
FROM ubuntu:18.04 as base

LABEL author=adrian.podsiadlowski@gmail.com
ENV DEBIAN_FRONTEND noninteractive

# Base toolchain
FROM base as base_tools
	RUN apt-get update && \
		apt-get -y upgrade && \
        apt-get install -y apt-utils && \
		apt-get install -y vim && \
		apt-get install -y sudo

# Base dev toolchain
FROM base_tools as dev_tools
	ARG DEBIAN_FRONTEND=noninteractive
	RUN apt-get update && \
		apt-get install -y git && \
		apt-get install -y gcc && \
		apt-get install -y g++ && \
		apt-get install -y gdb && \
		apt-get install -y cmake && \
		apt-get install -y libtbb-dev

# Code checkers
FROM dev_tools as checkers_tools
	RUN apt-get update && \
		apt-get install -y cppcheck && \
		apt-get install -y clang-tidy && \
		apt-get install -y clang-format

# Python tools
FROM checkers_tools as python_tools
	RUN apt-get update && \
		apt-get install -y python3 && \
		apt-get install -y python3-pip

# Conan package manager
FROM python_tools as conan_tools
	RUN pip3 install conan

# Raspberry pi kernel tools
FROM conan_tools as kernel_tools
    RUN apt-get update && \
        apt-get install -y device-tree-compiler && \
        apt-get install -y bc && \
        apt-get install -y bison && \
        apt-get install -y flex && \
        apt-get install -y libssl-dev && \
        apt-get install -y python3-ply && \
        apt-get install -y python3-git && \
        apt-get install -y libgmp3-dev && \
        apt-get install -y libmpc-dev && \
        apt-get install crossbuild-essential-armhf && \
		apt-get install -y libncurses5-dev


# User image
FROM kernel_tools as user_image
	RUN git config --global user.name Adrian_Podsiadlowski
	RUN git config --global user.email adrian.podsiadlowski@gmail.com
	WORKDIR /repo
	ENTRYPOINT /bin/bash
	CMD /