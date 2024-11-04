
FROM  python:3.11.10-bookworm

USER root


RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y build-essential \
    git \
    net-tools \
    arp-scan \
    tcpdump \
    ethtool \
    nmap \
    traceroute \
    iputils-ping \
    dnsutils \
    iptables \
    iproute2 \
    telnet \
    libnetfilter-queue-dev \
    build-essential \
    valgrind

WORKDIR /home/kmeans

COPY . .

RUN apt-get install -y g++ make
