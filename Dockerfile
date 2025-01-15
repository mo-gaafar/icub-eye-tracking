# Use Ubuntu Jammy as base image
FROM ubuntu:jammy

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    sudo \
    lsb-release \
    wget \
    gnupg \
    && rm -rf /var/lib/apt/lists/*

# Add iCub repository and GPG key
RUN apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 57A5ACB6110576A6 && \
    sh -c 'echo "deb http://www.icub.eu/ubuntu $(lsb_release -cs) contrib/science" > /etc/apt/sources.list.d/icub.list'

# Update package lists
RUN apt-get update

# Install iCub
RUN apt-get install -y icub

# Set working directory
WORKDIR /workspace

# Reset frontend
ENV DEBIAN_FRONTEND=dialog