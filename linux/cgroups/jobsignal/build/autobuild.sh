#!/bin/sh

################################################################################
# Avoid remembering all the automake toolchain, make will call this script
################################################################################

# copy source files
cd bin
cp -r ../jobsignal/src .
cp ../jobsignal/src/*.h .
cp ../jobsignal/build/configure.ac .
cp ../jobsignal/build/Makefile.am .

# using autotools
autoreconf --force --install
./configure
make

# made
cd ..
