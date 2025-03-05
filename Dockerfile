FROM python:3.8 

ARG DEBIAN_FRONTEND=noninteractive
ARG DOCKER_WORKSPACE=/home/developer

WORKDIR /home
USER root
# Get GNU ARM compiler 
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2 \
    && tar -xf gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2 

# Install mbed and dependencies
RUN apt update && apt install -y git mercurial && pip install mbed-cli 

WORKDIR ${DOCKER_WORKSPACE}

# Additional mbed dependencies. Standard mbed-os reqs cannot solve on some platforms - re-written with updated Jinja2, 
# cmsis-p-m, cryptography and pyyaml
COPY requirements.txt ./requirements.txt
RUN pip install -r requirements.txt && rm requirements.txt

# Configure mbed
RUN mbed config -G GCC_ARM_PATH /home/gcc-arm-none-eabi-10-2020-q4-major/bin

# Set a generic entrypoint rather than "mbed" because we often need to follow the compile action with additional commands in the container
ENTRYPOINT [ "/bin/sh", "-c" ]