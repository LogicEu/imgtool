#!/bin/bash

comp=gcc
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
    $comp ${flags[*]} ${inc[*]} ${lib[*]} -dynamiclib ${src[*]} -o lib$name.dylib
    install_name_tool -id @executable_path/lib$name.dylib lib$name.dylib 
}

linux_dlib() {
    $comp -shared ${flags[*]} ${inc[*]} ${lib[*]} -fPIC ${src[*]} -o lib$name.so 
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
    $comp ${flags[*]} ${inc[*]} -c ${src[*]}
    ar -crv lib$name.a *.o
    rm *.o
}

compile() {
    $comp *.c ${flags[*]} -I. -L. -l$name ${lib[*]} -o $name
}

clean() {
    rm $name
    rm lib$name.a
}

install() {
    mv $name /usr/local/bin/$name
}

if [[ $# < 1 ]]; then 
    fail_op
elif [[ "$1" == "-dlib" ]]; then
    dlib
elif [[ "$1" == "-slib" ]]; then
    slib
elif [[ "$1" == "-comp" ]]; then
    slib
    compile
elif [[ "$1" == "-install" ]]; then
    slib
    compile
    install
elif [[ "$1" == "-clean" ]]; then
    clean
else
    fail_op
fi 
