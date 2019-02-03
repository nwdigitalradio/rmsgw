noinst_SCRIPTS = do_subst

RMS_SYSCONFDIR = $(sysconfdir)/@PACKAGE@

etcfiles = acihelp\
        banner\
        channels.xml\
        channels.xsd\
        gateway.conf\
        gwhelp\
        hosts

SUBDIRS = . hooks

install-data-hook:
	##If there is an existing stat folder, remove it to force a status update
	@if test -d $(DESTDIR)$(RMS_SYSCONFDIR)/stat; then \
	  echo " $(DESTDIR)$(RMS_SYSCONFDIR)/stat already exists; emptying to force status update"; \
	  $(RM) -rf $(DESTDIR)$(RMS_SYSCONFDIR)/stat || true; \
	fi
	##If there is an existing rms.auth, remove is, it is no loner required
	@if test -f $(DESTDIR)$(RMS_SYSCONFDIR)/rms.auth; then \
	  echo " $(DESTDIR)$(RMS_SYSCONFDIR)/rms.auth is no longer required; deleting"; \
	  $(RM) -f $(DESTDIR)$(RMS_SYSCONFDIR)/rms.auth || true; \
	fi
	##Re-create the stat folder, or create one if this is a new install
	$(mkinstalldirs) $(DESTDIR)$(RMS_SYSCONFDIR)/stat
	##On a new install, install the config files with their default names. 
	##On an upgeade, if we find files with the same name, install these ones 
	##with a .dist suffix, so we don't overwrite any user changed files.
	@for p in $(etcfiles); do \
	  if test -f $(DESTDIR)$(RMS_SYSCONFDIR)/$$p; then \
	  echo " $(DESTDIR)$(RMS_SYSCONFDIR)/$$p already exists; installing new file as $$p.dist"; \
	  $(INSTALL_DATA) $$p $(DESTDIR)$(RMS_SYSCONFDIR)/$$p.dist; \
	  else \
	  echo " Existing $$p not found; installing default $$p"; \
	  $(INSTALL_DATA) $$p $(DESTDIR)$(RMS_SYSCONFDIR)/$$p; \
	  fi; \
	done

#This makes distcheck happy by deleting the /etc/rmsgw folder on make uninstall
uninstall-hook:
	$(RM) -rf $(DESTDIR)$(RMS_SYSCONFDIR)

#This will replace variables in the .in files when make processes them
edit = sed \
	-e 's|@sbindir[@]|$(sbindir)|g' \
	-e 's|@bindir[@]|$(bindir)|g' \
	-e 's|@sysconfdir[@]|$(sysconfdir)|g' \
	-e 's|@localstatedir[@]|$(localstatedir)|g' \
	-e 's|@prefix[@]|$(prefix)|g'

do_subst:
	@list='$(etcfiles)'; for p in $$list; do \
	$(edit) $(srcdir)/$$p.in >$$p; \
	done

CLEANFILES = $(etcfiles)

EXTRA_DIST = acihelp.in\
        banner.in\
        channels.xml.in\
        channels.xsd.in\
        gateway.conf.in\
        gwhelp.in\
        hosts.in
