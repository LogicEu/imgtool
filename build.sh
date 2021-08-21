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
)

inc=(
    -I./
    -Iinclude/
)

lib=(
    -lz
    -lpng
    -ljpeg
)

fail_op() {
    echo "Use -dlib to build the library dynamically"
    echo "Use -slib to build the library statically"
    echo "Use -comp to compile the imgtool terminal tool"
    echo "Use -install to install the imgtool in your system"
    exit
}

fail_os() {
    echo "OS is not supported yet..."
    exit
}

mac_dlib() {
    $cc ${flags[*]} ${inc[*]} ${lib[*]} -dynamiclib ${src[*]} -o lib$name.dylib &&\
    install_name_tool -id @executable_path/lib$name.dylib lib$name.dylib 
}

linux_dlib() {
    $cc -shared ${flags[*]} ${inc[*]} ${lib[*]} -fPIC ${src[*]} -o lib$name.so 
}

dlib() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        mac_dlib
    elif echo "$OSTYPE" | grep -q "linux"; then
        linux_dlib
    else
        fails_os
    fi
}

slib() {
    $cc ${flags[*]} ${inc[*]} -c ${src[*]} && ar -crv lib$name.a *.o && rm *.o
}

compile() {
    $cc *.c ${flags[*]} -I. -L. -l$name ${lib[*]} -o $name
}

clean() {
    rm $name
    rm lib$name.a
}

install() {
    sudo cp $name /usr/local/bin/$name
}

case "$1" in
    "-dlib")
        dlib;;
    "-slib")
        slib;;
    "-comp")
        slib && compile;;
    "-install")
        slib && compile && install;;
    "-clean")
        clean;;
    *)
        fail_op;;
esac
