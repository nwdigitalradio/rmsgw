docdir = $(datadir)/doc/@PACKAGE@
doc_DATA = rmsgwchantest.ref

etcdir = $(sysconfdir)/@PACKAGE@
etc_PROGRAMS = rmsgwchantest

rmsgwchantest_SOURCES = rmsgwchantest.c

rmsgwchantest_LDADD = ../lib/librms.a $(XML_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/include \
	-I$(top_srcdir)/lib \
	$(XML_CFLAGS) \
	-D_GNU_SOURCE \
	-DGWMONITOR

EXTRA_DIST = $(doc_DATA)
