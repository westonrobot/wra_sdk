## Build the sample code

Install wra_sdk and its dependencies:

```
$ sudo apt remove wrp_zbus wrp_sdk
$ sudo apt install wrp_zbus=0.6.1
$ sudo apt install wrp_sdk=0.8.9
$ sudo apt install wra_sdk
```

Build the sample code:

```
$ mkdir build && cd build
$ cmake ..
$ make -j8
```
