# FROM ubuntu:18.04

# # RUN echo "deb http://de.archive.ubuntu.com/ubuntu precise main restricted universe" >> /etc/apt/sources.list
# # RUN echo "deb-src http://de.archive.ubuntu.com/ubuntu precise restricted main multiverse universe" >> /etc/apt/sources.list
# # RUN echo "" >> /etc/apt/sources.list
# # RUN echo "deb http://de.archive.ubuntu.com/ubuntu precise-updates main restricted universe  " >> /etc/apt/sources.list
# # RUN echo "deb-src http://de.archive.ubuntu.com/ubuntu precise-updates restricted main multiverse universe" >> /etc/apt/sources.list

# RUN apt-get update && apt-get -y upgrade
# RUN apt install -y software-properties-common apt-file
# RUN apt-get install -y build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev 
# RUN apt-get install -y libtbb2 libtbb-dev
# RUN apt-get install -y build-essential cmake
# RUN apt-get install -y libopencv-core-dev libopencv-highgui-dev libopencv-imgcodecs-dev libopencv-imgproc-dev libjsoncpp-dev
# RUN apt-get install -y libjpeg-dev libpng-dev libtiff5-dev libjasper-dev libdc1394-22-dev libeigen3-
# # RUN apt-add-repository ppa:deadsnakes/ppa

# # RUN apt-get update
# WORKDIR /code
# COPY . /code/
# # RUN rm -rf build/ bin/ lib/ && mkdir build
# # WORKDIR /code/build
# # RUN cmake -DCMAKE_C_COMPILER="gcc" -DCMAKE_CXX_COMPILER="g++" .. && make
# # WORKDIR /code/
# # RUN ls
# CMD ["ls"]

FROM veerabadralokesh/ubuntu2004-googlecloud-python3-opencv:2

ARG OPENCV_VERSION="3.0.0"

# install dependencies
RUN apt-get update
RUN apt-get install -y libopencv-dev yasm libjpeg-dev libjasper-dev libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev libv4l-dev python-dev python-numpy libtbb-dev libqt4-dev libgtk2.0-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils pkg-config
RUN apt-get install -y curl build-essential checkinstall cmake

# download opencv
RUN curl -sL https://github.com/Itseez/opencv/archive/$OPENCV_VERSION.tar.gz | tar xvz -C /tmp
RUN mkdir -p /tmp/opencv-$OPENCV_VERSION/build

WORKDIR /tmp/opencv-$OPENCV_VERSION/build

# install
RUN cmake -DWITH_FFMPEG=OFF -DWITH_OPENEXR=OFF -DBUILD_TIFF=OFF -DWITH_CUDA=OFF -DWITH_NVCUVID=OFF -DBUILD_PNG=OFF ..
RUN make
RUN make install

# configure
RUN echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf
RUN ldconfig
RUN ln /dev/null /dev/raw1394 # hide warning - http://stackoverflow.com/questions/12689304/ctypes-error-libdc1394-error-failed-to-initialize-libdc1394

# cleanup package manager
# RUN apt-get remove --purge -y curl build-essential checkinstall cmake
# RUN apt-get autoclean && apt-get clean
# RUN rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# prepare dir
RUN apt-get install -y libopencv-core-dev libopencv-highgui-dev libopencv-imgcodecs-dev libopencv-imgproc-dev libjsoncpp-dev

RUN mkdir /source

# VOLUME ["/source"]
WORKDIR /source
COPY . /source/
RUN rm -rf build && rm -rf bin && rm -rf lib && mkdir build
WORKDIR /source/build
RUN cmake -DCMAKE_C_COMPILER="gcc" -DCMAKE_CXX_COMPILER="g++" ../ && make
CMD ["bash"]