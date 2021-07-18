- boost 1.72
- msvc 16 2019

how to build Boost on linux: https://www.boost.org/doc/libs/1_72_0/more/getting_started/unix-variants.html

```
check list if libraries required to build:
./bootstrap.sh --show-libraries

configure to build serialization only and set directory to install sources
./bootstrap.sh --with-libraries=serialization --prefix=/home/olehk/extra/boost/boost172

actually build...
/b2 --build-dir=/home/olehk/extra/boost/boost172/build variant=release link=static threading=multi runtime-link=static

and install
./b2 install
```
