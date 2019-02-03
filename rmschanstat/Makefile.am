BUILT_SOURCES = do_subst

bin_SCRIPTS = rmschanstat

edit = sed \
	-e 's|@sbindir[@]|$(sbindir)|g' \
	-e 's|@sysconfdir[@]|$(sysconfdir)|g' \
	-e 's|@prefix[@]|$(prefix)|g' \
	-e 's|@datadir[@]|$(datadir)|g'

do_subst:
	@list='$(bin_SCRIPTS)'; for p in $$list; do \
	$(edit) $(srcdir)/$$p.in >$$p; \
	done

CLEANFILES = $(bin_SCRIPTS)

EXTRA_DIST = rmschanstat.in
