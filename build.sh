#!/bin/bash

cc=gcc
src=src/*.c
name=imgtool
lib=lib$name

flags=(
    -std=c99
    -Wall
    -Wextra
    -pedantic
    -O2
    -I.
)

libs=(
    -lz
    -lpng
    -ljpeg
)

if echo "$OSTYPE" | grep -q "darwin"; then
    dlib=(
        -dynamiclib
    )
    suffix=.dylib
elif echo "$OSTYPE" | grep -q "linux"; then
    dlib=(
        -shared
        -fPIC
    )
    suffix=.so
else
    echo "This OS is not supported by this shell script yet..." && exit
fi

cmd() {
    echo "$@" && $@
}

exe() {
    [ ! -f bin/$lib.a ] && echo "Use 'static' before building executable" && exit
    cmd $cc $name.c -o $name ${flags[*]} -Lbin -l$name ${libs[*]}
}

shared() {
    cmd mkdir -p tmp
    cmd $cc -c $src ${flags[*]} && cmd mv *.o tmp/ || exit
    
    cmd mkdir -p bin
    cmd $cc tmp/*.o -o bin/$lib$suffix ${libs[*]} ${dlib[*]}
}

static() {
    cmd mkdir -p tmp
    cmd $cc ${flags[*]} -c $src && cmd mv *.o tmp/ || exit
    
    cmd mkdir -p bin
    cmd ar -cr bin/$lib.a tmp/*.o
}

cleand() {
    [ -d $1 ] && cmd rm -r $1
}

cleanf() {
    [ -f $1 ] && cmd rm $1
}

clean() {
    cleanf $name
    cleand bin
    cleand tmp
    return 0
}

install() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to install" && exit
    
    make all -j # or static && shared && exe
    cmd cp $name.h /usr/local/include/
    cmd cp $name  /usr/local/bin/

    [ -f bin/$lib.a ] && cmd mv bin/$lib.a /usr/local/lib
    [ -f bin/$lib.so ] && cmd mv bin/$lib.so /usr/local/lib
    [ -f bin/$lib.dylib ] && cmd mv bin/$lib.dylib /usr/local/lib
    
    echo "Successfully installed $name"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to uninstall" && exit

    cleanf /usr/local/bin/$name
    cleanf /usr/local/include/$name.h
    cleanf /usr/local/lib/$lib.a
    cleanf /usr/local/lib/$lib.so
    cleanf /usr/local/lib/$lib.dylib

    echo "Successfully uninstalled $name"
    return 0
}

case "$1" in
    "shared")
        shared;;
    "static")
        static;;
    "exe")
        exe;;
    "all")
        shared && static && exe;;
    "make")
        make all -j;;
    "clean")
        clean;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    *)
        echo "Run with 'shared', 'static' or 'exe' to build library or executable"
        echo "Use 'install' to build and install in /usr/local"
        echo "Use 'clean' to remove local builds"
esac
