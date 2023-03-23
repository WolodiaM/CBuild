#!/bin/bash
# Rebuild CBuild user scripts
# $1 - user scripts dir
# $2 - custom user build args args in format "-arg1 -arg2"
# $3 - custom user link args in format "-arg1 -arg2"
# $4 - custom name of user executable
# Error codes
# 1 - Too litle number of elements
# 2 - Too many arguments
# 3 - Too litle number of function arguments
# Vars
C_PATH=""
SCRIPTS_DIR=""
PLUGIN_INC=""
COMPILE_ARG=""
LINK_ARG=""
NAME="CBuild.run"
# Throw error and exit
# $1 => string -> Error descriptions
# $2 => int -> Error code
function error() {
    if [ $# -lt 2 ]; then 
        error "Error in function error() -> need 2 argument but received less" 3;
    fi
    echo "$1";
    echo "Exiting with error code $2 ...";
    exit $2;
}
# ELF path and scripts dir path need to be specified
if [ $# -lt 1 ]; then
    error "Need to specify user executable path and user script dir path." 1;
fi
# Needed path
C_PATH=$1
# Custom compile args exists
if [ $# -gt 1 ]; then
    COMPILE_ARG=$2;
fi
# Custom link args exists
if [ $# -gt 2 ]; then
    LINK_ARG=$3;
fi
# Name is specified
if [ $# -gt 3 ]; then
    NAME=$4;
fi
# Too many args
if [ $# -gt 4 ]; then
    error "Too many arguments." 2;
fi
# Get files needed for compilation
cd ${C_PATH}/../
SCRIPTS_DIR=`basename ${C_PATH}`
scripts=`dir ${SCRIPTS_DIR}/*.cpp`
# Compile
link="-lCBuild -Wl,-z,origin -Wl,-rpath,\$ORIGIN/CBuild/CBuild ${PLUGIN_INC} ${LINK_ARG}"
g++ -c ${scripts} ${COMPILE_ARG} -fPIC -std=c++20 -Wall -Wextra -Wno-comments ${link}
objects=`dir *.o`
g++ ${objects} ${link} -o ${NAME}
rm -r ${objects}