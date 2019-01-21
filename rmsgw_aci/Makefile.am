bin_PROGRAMS = rmsgw_aci

rmsgw_aci_SOURCES = rmsgw_aci.c \
	run_python_script.c \
	wl2k_aci.c \
	sendversion.c \
	sendchannel.c \
	glob.c

rmsgw_aci_LDADD = ../lib/librms.a $(XML_LIBS)

AM_CPPFLAGS = -I$(top_srcdir)/include \
        -I$(top_srcdir)/lib \
        $(XML_CFLAGS) \
        -D_GNU_SOURCE \
        -DGWMONITOR \
	-DRMS_SYSCONFDIR=\""$(sysconfdir)/rmsgw/"\"

RMS_SYSCONFDIR = @sysconfdir@/rmsgw/
