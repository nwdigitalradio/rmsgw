bin_PROGRAMS = rmsgw
rmsgw_SOURCES = rmsgw.c \
	gwinit.c \
	greet.c \
	banner.c \
	session.c \
	gateway.c \
	log_login.c \
	log_logout.c \
	glob.c

rmsgw_LDADD = ../lib/librms.a $(XML_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/include \
        -I$(top_srcdir)/lib \
        $(XML_CFLAGS) \
        -D_GNU_SOURCE \
        -DGWMONITOR \
	-DRMS_SYSCONFDIR=\""$(sysconfdir)/rmsgw/"\"

RMS_SYSCONFDIR = @sysconfdir@/rmsgw/
