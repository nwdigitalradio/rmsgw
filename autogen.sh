#!/bin/sh


echo "---removing generated files---"
# products from ./configure and make
if test -f Makefile ; then
# touch all automatically generated targets that we do not want to rebuild
touch aclocal.m4
touch configure
touch Makefile.in
touch config.status
touch Makefile
# make maintainer-clean
make -k maintainer-clean
fi

# products from autoreconf
rm -Rf autom4te.cache
rm -f ./m4/*
rm -f Makefile.in aclocal.m4 compile config.guess config.sub configure \
      config.h.in depcomp install-sh ltconfig ltmain.sh missing ar-lib
rm -f `find . -name "Makefile.in"`


if test "$1" != "clean" ; then
  # bootstrap
  echo "---autoreconf---"
  autoreconf -v -i -f
fi

