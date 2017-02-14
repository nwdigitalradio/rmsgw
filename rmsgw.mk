#
# build dir
#
ifeq ($(LEVEL), 1)
TOPDIR	:= $(CURDIR)/..
else
TOPDIR	:= $(CURDIR)
endif

#
# set your platform: LINUX, ???
#
PLATFORM	:= LINUX

#
# Set the name of the compiler
#
CC	:= gcc

#
# user and group for installed files
#
INSTUSR	:= rmsgw
INSTGRP	:= rmsgw

INSTDIR	:= /usr/local

GFLAG	:= -g
#SSPFLAG	:= -fno-stack-protector
SSPFLAG		:=
FEATDEFN	:= -D_GNU_SOURCE -DGWMONITOR
#FEATDEFN	:= -D_GNU_SOURCE -DGWMONITOR -DHOOKS
#FEATDEFN	:=
ARCH	:= $(shell uname -m)
PKG_DEFINES	:= -D$(PLATFORM) $(FEATDEFN) $(SSPFLAG)
PKG_CFLAGS	:= $(GFLAG)
PKG_LDFLAGS	:= $(GFLAG)
#PKG_LIBS	:= -lwl2k -lrms -lmysqlclient -lxml2
PKG_LIBS	:= -lrms -lxml2
PKG_INCLUDES	:= -I$(TOPDIR)/include -I/usr/include/libxml2
PKG_LIBDIR	:= $(TOPDIR)/lib
PKG_LIBDIRS	:= -L$(PKG_LIBDIR)
