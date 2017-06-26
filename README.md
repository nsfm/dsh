# dsh
A simple, modular shell implemented in C++11.

Tested builds with:
gcc 6.1.1, arm-linux-gcc 4.8.1
GNU Make 4.2.1

Tested on:
Arch GNU/Linux 4.6.4-1 x86_64

# Compiling
To build for local use, run `make`.
To clean up your build environment, run `make clean`.
The `dsh` binary will be built in the root project directory.

# Installing
Stay tuned.

# Testing
Stay tuned.

# Cross Compile
To cross compile, it should be sufficient to export the name of your cross compiler as `CXX`.
Ex: `export CXX="arm-linux-gcc"`, then `make`
Ex: `CXX=arm-linux-gcc make`

Tested with gcc. No other compilation tools are specifically used.
See the Makefile for additional overrides.

# Disclaimer
Don't deploy this to anything as an actual shell. Not intended for daily use.
