BUILT_SOURCES = do_subst

pathheader = pathnames.h

edit = sed \
	-e 's|@rmssysconfdir[@]|$(RMS_SYSCONFDIR)|g'

do_subst:
	@list='$(pathheader)'; for p in $$list; do \
	$(edit) $(srcdir)/$$p.in >$$p; \
	done

CLEANFILES = $(pathheader)

SUBDIRS = include lib rmsgw rmsgw_aci rmsgwmon rmschanstat devutils man contrib doc etc

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = pathnames.h.in

RMS_SYSCONFDIR = @sysconfdir@/@PACKAGE@/
