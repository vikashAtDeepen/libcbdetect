FROM ubuntu:xenial

RUN echo "deb http://de.archive.ubuntu.com/ubuntu precise main restricted universe" >> /etc/apt/sources.list
RUN echo "deb-src http://de.archive.ubuntu.com/ubuntu precise restricted main multiverse universe" >> /etc/apt/sources.list
RUN echo "" >> /etc/apt/sources.list
RUN echo "deb http://de.archive.ubuntu.com/ubuntu precise-updates main restricted universe  " >> /etc/apt/sources.list
RUN echo "deb-src http://de.archive.ubuntu.com/ubuntu precise-updates restricted main multiverse universe" >> /etc/apt/sources.list

# RUN apt-get update
# RUN apt-get install -y libopencv-core-dev libopencv-highgui-dev libopencv-imgcodecs-dev libopencv-imgproc-dev

WORKDIR /code
COPY . /code/

CMD ["ls"]