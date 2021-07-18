- boost 1.72
- msvc 16 2019

how to build Boost on linux: https://www.boost.org/doc/libs/1_72_0/more/getting_started/unix-variants.html

```
check list of libraries
./bootstrap.sh --show-libraries

configure to build specific libs only and set directory to install sources
./bootstrap.sh --with-libraries=serialization,system,thread,filesystem,program_options --prefix=/home/olehk/extra/boost/boost172

build
./b2 --build-type=complete --layout=tagged stage

install
./b2 --build-type=complete --layout=tagged install
```
