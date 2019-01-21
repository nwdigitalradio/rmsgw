bin_PROGRAMS = rmsgwmon

rmsgwmon_SOURCES = rmsgwmon.c \
	glob.c 

rmsgwmon_LDADD = ../lib/librms.a $(XML_LIBS) $(NCURSES_LIB)

AM_CPPFLAGS = -I$(top_srcdir)/include \
	-I$(top_srcdir)/lib \
	$(XML_CFLAGS) \
	-D_GNU_SOURCE \
	-DGWMONITOR \
	-DRMS_SYSCONFDIR=\""$(sysconfdir)/rmsgw/"\"

RMS_SYSCONFDIR = @sysconfdir@/rmsgw/
