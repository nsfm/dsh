# dsh
A simple, modular shell implemented in C++11.

*Tested builds with:* <br/>
gcc 6.1.1, arm-linux-gcc 4.8.1, clang-700.1.81<br/>
GNU Make 4.2.1

*Tested on:* <br/>
Arch GNU/Linux 4.6.4-1 x86_64 <br/>
OS X 14.5.0 x86_64

Please note: dsh is still very early in development! Currently it can perform basic shell operations, but my goal is to add a plugin loading system with hooks into various internals.

## Compiling
To build for local use, run `make`. <br/>
To clean up your build environment, run `make clean`. <br/>
The `dsh` binary will be built in the root project directory.

## Installing
Stay tuned.

## Testing
Stay tuned.

## Cross Compile
To cross compile, it should be sufficient to export the name of your cross compiler as `CXX`. <br/>
Ex: `export CXX="arm-linux-gcc"`, then `make` <br/>
Ex: `CXX=arm-linux-gcc make`

No other compilation tools are specifically used. <br/>
See the Makefile for additional overrides.

## Disclaimers
Don't deploy this to anything. Not intended for daily use. See the LICENSE for additional restrictions.
