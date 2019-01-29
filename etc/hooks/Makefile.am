noinst_SCRIPTS = do_subst

RMS_SYSCONFDIR = $(sysconfdir)/@PACKAGE@/hooks

etcfiles = \
	end-rmsgw\
	post-rmsgw_aci-channel-update\
	post-rmsgw-session\
	pre-rmsgw_aci-version-update\
	start-rmsgw\
	end-rmsgw_aci\
	post-rmsgw_aci-version-update\
	pre-rmsgw_aci-channel-update\
	pre-rmsgw-session\
	start-rmsgw_aci

install-data-hook:
	$(mkinstalldirs) $(DESTDIR)$(RMS_SYSCONFDIR)
	@for p in $(etcfiles); do \
	  if test -f $(DESTDIR)$(RMS_SYSCONFDIR)/$$p; then \
	  echo " $(DESTDIR)$(RMS_SYSCONFDIR)/$$p already exists; installing new file as $$p.dist"; \
	  $(INSTALL_DATA) $$p $(DESTDIR)$(RMS_SYSCONFDIR)/$$p.dist; \
	  else \
	  echo " Existing $$p not found; installing default $$p"; \
	  $(INSTALL_DATA) $$p $(DESTDIR)$(RMS_SYSCONFDIR)/$$p; \
	  fi; \
	done
	
edit = sed \
	-e 's|@sbindir[@]|$(sbindir)|g' \
	-e 's|@sysconfdir[@]|$(sysconfdir)|g' \
	-e 's|@localstatedir[@]|$(localstatedir)|g' \
	-e 's|@prefix[@]|$(prefix)|g'

do_subst:
	@list='$(etcfiles)'; for p in $$list; do \
	$(edit) $(srcdir)/$$p.in >$$p; \
	done

CLEANFILES = $(etcfiles)

EXTRA_DIST = \
	end-rmsgw.in\
	post-rmsgw_aci-channel-update.in\
	post-rmsgw-session.in\
	pre-rmsgw_aci-version-update.in\
	start-rmsgw.in\
	end-rmsgw_aci.in\
	post-rmsgw_aci-version-update.in\
	pre-rmsgw_aci-channel-update.in\
	pre-rmsgw-session.in\
	start-rmsgw_aci.in

