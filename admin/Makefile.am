BUILT_SOURCES = do_subst

RMS_SYSCONFDIR = $(sysconfdir)/@PACKAGE@

etcdir = $(RMS_SYSCONFDIR)
etc_SCRIPTS = \
	mksysop.py \
	getsysop.py \
	getchannel.py \
	updatechannel.py \
	updatesysop.py \
	updateversion.py \
	getversionlist.py \
	versionlist.sh \
	testwlapi.sh \
	admin-update.sh

dist_etc_DATA = \
	winlinkservice.xml \
	sysop-template.xml \
	.version_info

dist_doc_DATA = README.md

install-data-hook:
	##Modify some permissions per the original source.
	chmod 0660 $(DESTDIR)$(RMS_SYSCONFDIR)/winlinkservice.xml
	chmod 0660 $(DESTDIR)$(RMS_SYSCONFDIR)/sysop-template.xml
	chmod 0664 $(DESTDIR)$(RMS_SYSCONFDIR)/.version_info	

#This will replace variables in the .in files when make processes them
edit = sed \
	-e 's|@sbindir[@]|$(sbindir)|g' \
	-e 's|@bindir[@]|$(bindir)|g' \
	-e 's|@sysconfdir[@]|$(sysconfdir)|g' \
	-e 's|@localstatedir[@]|$(localstatedir)|g' \
	-e 's|@prefix[@]|$(prefix)|g' \
	-e 's|@rmssysconfdir[@]|$(RMS_SYSCONFDIR)|g'

do_subst:
	@list='$(etc_SCRIPTS)'; for p in $$list; do \
	$(edit) $(srcdir)/$$p.in >$$p; \
	done

CLEANFILES = $(etc_SCRIPTS)

EXTRA_DIST = \
	mksysop.py.in \
	getsysop.py.in \
	getchannel.py.in \
	updatechannel.py.in \
	updatesysop.py.in \
	updateversion.py.in \
	getversionlist.py.in \
	versionlist.sh.in \
	testwlapi.sh.in \
	admin-update.sh.in

