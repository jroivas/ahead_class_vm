#!/bin/bash

fname=$1
if [ ! -e "${fname}" ] ; then
    echo "Usage: $0 fname.class [fname2.class] [fnameN.class]"
    exit 1
fi
bname=$(basename "${fname}" .class)

ninja
./src/ahead_class_vm $@ > "${bname}.cpp"
#g++ -O3 -I../inc -I../lib "${bname}.cpp" -o "${bname}" -Llib -lclass -lgc -ljemalloc
g++ -O3 -I../inc -I../lib "${bname}.cpp" -o "${bname}" -Llib -lclass -lgc
