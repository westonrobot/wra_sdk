# Weston Robot Application SDK (wra_sdk)

wra_sdk provides C++ interface to autonomous navigation and application development functionalities provided by the robot platforms from Weston Robot. wra_sdk is provided as a **Debian installation package** only and you can install it by following the steps given below. For sample code, please check the "sample-*" branches of this repository. 

## Supported platforms

* OS: Ubuntu 18.04, Ubuntu 20.04
* Architecture: x86_64

## Install the SDK

wra_sdk requires the following dependencies:

* OpenCV: libopencv-dev

```
$ sudo apt install libopencv-dev
```

* gRPC: v3.11 or newer

```
$ sudo apt-get update
$ sudo apt-get install apt-transport-https ca-certificates gnupg software-properties-common wget
$ wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
$ sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
$ sudo apt-get update
$ sudo apt-get install kitware-archive-keyring
$ sudo rm /etc/apt/trusted.gpg.d/kitware.gpg
$ sudo apt-get install cmake
$ sudo apt install -y build-essential autoconf libtool pkg-config
$ cd ~
$ git clone --recurse-submodules -b v1.36.2 https://github.com/grpc/grpc
$ cd grpc
$ mkdir -p build
$ pushd build
$ cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -Dprotobuf_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=/opt/weston_robot \
      ..
$ make -j
$ make install
$ popd
```

Now install the SDK package. Note that if you've installed **wr_assisted_teleop**, you likely have already added the Weston Robot apt source. In such a case, you can skip the first two lines of commands.

```
$ echo "deb https://westonrobot.jfrog.io/artifactory/wrtoolbox-release $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/weston-robot.list
$ curl -sSL 'https://westonrobot.jfrog.io/artifactory/api/security/keypair/wr-deb/public' | sudo apt-key add -
$ sudo apt-get update
$ sudo apt-get install wra_sdk
```

**Note**: "apt-get" will install the latest version of wra_sdk (and its dependencies) by default. Weston Robot releases stable versions of the SDK twice a year. In case the robot hardware and pre-installed services are not compatible with the latest SDK, you can install older versions of the SDK manually. Please contact us if you're not sure which version of the SDK you should use.