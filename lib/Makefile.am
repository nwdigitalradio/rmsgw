noinst_LIBRARIES = librms.a

librms_a_SOURCES = \
	fileage.c \
	syslogmap.c \
	cmsconnect.c \
	loadconfig.c \
	getcms.c \
	getcmslist.c \
	cmsnode.c \
	statfile.c \
	setcmsstat.c \
	cmslogin.c \
	upcase.c \
	downcase.c \
	strcvt.c \
	fgetline.c \
	fgetlinecr.c \
	err.c \
	mapname.c \
	sendrf.c \
	printfile.c \
	getchan.c \
	set.c \
	symbol.c \
	var.c \
	strtrim.c \
	senddatagram.c \
	shm.c \
	statblock.c \
	md5.c \
	readln.c \
	sglchallenge.c \
	loadversion.c \
	loadenv.c \
	file_exists.c \
	runhook.c


#librms_a_LDFLAGS = $(XML_LIBS)

librms_a_CPPFLAGS = -I$(top_srcdir)/include \
	$(XML_CFLAGS) \
	-D_GNU_SOURCE \
	-DGWMONITOR
