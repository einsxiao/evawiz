#!/bin/bash
str="$0"
dir=${str:2:(-3)}
if ! [ -d $dir ]; then echo "Exit for 'Dir $dir not exist'"; exit 0; fi
if ! [ -d $dir-build ]; then mkdir $dir-build; fi
cd $dir-build

../$dir/configure --prefix=$EVAWIZ_ROOT/gdb \
                      --with-system-gdbinit=$EVAWIZ_ROOT/etc/gdb/gdbinit \
                      --with-gdb-datadir=$EVAWIZ_ROOT/etc/gdb/data \
                      --with-python \

make -j 4
make install
exit 0

export LDFLAGS="-Wl,-rpath,$EVAWIZ_ROOT/python/python/lib -L$EVAWIZ_ROOT/python/lib"
../gdb-7.11/configure --prefix=$EVAWIZ_ROOT/gdb \
                      --with-system-gdbinit=$EVAWIZ_ROOT/etc/gdb/gdbinit \
			                --with-mpc=$EVAWIZ_ROOT/base/mpc \
			                --with-mpfr=$EVAWIZ_ROOT/base/mpfr \
			                --with-gmp=$EVAWIZ_ROOT/base/gmp \
                      --with-python \
                      --with-gdb-datadir=$EVAWIZ_ROOT/etc/gdb/data 

