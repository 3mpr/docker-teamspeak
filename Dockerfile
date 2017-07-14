FROM 3mpr/baseimage:alpine-glibc

ENV TEAMSPEAK_VERSION 3.0.13

# Add teamspeak group & user
RUN addgroup -g 503 teamspeak \
 && adduser -u 503 -G teamspeak -h /usr/local/share/teamspeak -S -D teamspeak

# Download & unpack Teamspeak and create teamspeak directories
RUN wget http://dl.4players.de/ts/releases/${TEAMSPEAK_VERSION}/teamspeak3-server_linux_amd64-${TEAMSPEAK_VERSION}.tar.bz2 -O /tmp/teamspeak.tar.bz2 \
 && tar jxf /tmp/teamspeak.tar.bz2 -C /usr/local/share/ \
 && mv /usr/local/share/teamspeak3-server_*/* /usr/local/share/teamspeak \
 && rm /tmp/teamspeak.tar.bz2 \
 && chown -R teamspeak:teamspeak /usr/local/share/teamspeak \
 && mkdir -p /var/log/teamspeak && chown -R teamspeak:teamspeak /var/log/teamspeak \
 && mkdir /etc/teamspeak && chown -R teamspeak:teamspeak /etc/teamspeak

# MariaDB library linking
RUN apk add --update --no-cache mariadb-client procps \
 && cp /usr/local/share/teamspeak/redist/libmariadb.so.2 /lib64 \
 && export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/lib64"

# Add starting script
ADD ["teamspeak3", "/usr/local/bin"]
ADD ["ts3_conf.ini", "/tmp"]
RUN chmod +x /usr/local/bin/teamspeak3

EXPOSE 9987/udp 10011 30033 41144
VOLUME /etc/teamspeak
VOLUME /var/log/teamspeak
WORKDIR /opt/teamspeak/
USER teamspeak

ENTRYPOINT ["/usr/local/bin/dumb-init", "teamspeak3"]
