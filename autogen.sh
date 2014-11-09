#!/bin/sh
#
# Run this script to generate Makefile skeletons and configure
# scripts.
#

libtoolize -f --copy
aclocal $*
autoheader
automake --add-missing --force-missing --copy
autoconf
