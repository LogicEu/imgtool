#!/bin/bash

cc=gcc
name=imgtool

src=(
    src/*.c
    src/gif/*.c
)

flags=(
    -std=c99
    -Wall
    -Wextra
    -O2
    -I.
)

lib=(
    -lz
    -lpng
    -ljpeg
)

shared() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        $cc ${flags[*]} ${lib[*]} -dynamiclib ${src[*]} -o lib$name.dylib
    elif echo "$OSTYPE" | grep -q "linux"; then
        $cc -shared ${flags[*]} ${lib[*]} -fPIC ${src[*]} -o lib$name.so 
    else
        echo "This OS is not supported yet..." && exit
    fi
}

static() {
    $cc ${flags[*]} -c ${src[*]} && ar -cr lib$name.a *.o && rm *.o
}

compile() {
    $cc $name.c ${flags[*]} -L. -l$name ${lib[*]} -o $name
}

cleanf() {
    [ -f $1 ] && rm $1 && echo "deleted $1"
}

clean() {
    cleanf $name
    cleanf lib$name.a
    cleanf lib$name.so
    cleanf lib$name.dylib
    return 0
}

install() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to install." && exit

    shared && static && compile

    cp $name.h /usr/local/include/

    [ -f $name ] && mv $name /usr/local/bin/
    [ -f lib$name.a ] && mv lib$name.a /usr/local/lib/
    [ -f lib$name.so ] && mv lib$name.so /usr/local/lib/
    [ -f lib$name.dylib ] && mv lib$name.dylib /usr/local/lib/

    echo "Successfully installed $name."
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "Run with sudo to uninstall." && exit

    cleanf /usr/local/bin/$name
    cleanf /usr/local/include/$name.h
    cleanf /usr/local/lib/lib$name.a
    cleanf /usr/local/lib/lib$name.so
    cleanf /usr/local/lib/lib$name.dylib

    echo "Successfully uninstalled $name."
    return 0
}

case "$1" in
    "shared")
        shared;;
    "static")
        static;;
    "comp")
        shared && compile;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    "clean")
        clean;;
    *)
        echo "Use 'shared' or 'static' to build the library."
        echo "Use 'comp' to compile the imgtool terminal tool."
        echo "Use 'install' to build and install imgtool in /usr/local."
        echo "Use 'clean' to remove local builds.";;
esac
