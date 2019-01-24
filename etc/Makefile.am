
etcfiles = acihelp\
	banner\
	channels.xml\
	channels.xsd\
	gateway.conf\
	gwhelp\
	hosts\
	hooks/end-rmsgw\
	hooks/post-rmsgw_aci-channel-update\
	hooks/post-rmsgw-session\
	hooks/pre-rmsgw_aci-version-update\
	hooks/start-rmsgw\
	hooks/end-rmsgw_aci\
	hooks/post-rmsgw_aci-version-update\
	hooks/pre-rmsgw_aci-channel-update\
	hooks/pre-rmsgw-session\
	hooks/start-rmsgw_aci

install-data-hook:
	@if test -d $(DESTDIR)$(RMS_SYSCONFDIR)/stat; then \
	  echo " $(DESTDIR)$(RMS_SYSCONFDIR)/stat already exists; emptying to force status update"; \
	  $(RM) -rf $(DESTDIR)$(RMS_SYSCONFDIR)/stat || true; \
	fi
	@if test -f $(DESTDIR)$(RMS_SYSCONFDIR)/rms.auth; then \
	  echo " $(DESTDIR)$(RMS_SYSCONFDIR)/rms.auth is no longer required; deleting"; \
	  $(RM) -f $(DESTDIR)$(RMS_SYSCONFDIR)/rms.auth || true; \
	fi
	$(mkinstalldirs) $(DESTDIR)$(RMS_SYSCONFDIR)
	$(mkinstalldirs) $(DESTDIR)$(RMS_SYSCONFDIR)/hooks
	$(mkinstalldirs) $(DESTDIR)$(RMS_SYSCONFDIR)/stat
	@list='$(etcfiles)'; for p in $$list; do \
	  if test -f $(DESTDIR)$(RMS_SYSCONFDIR)/$$p; then \
	  echo " $(DESTDIR)$(RMS_SYSCONFDIR)/$$p already exists; installing as $$p.dist"; \
	  $(INSTALL_DATA) $$p $(DESTDIR)$(RMS_SYSCONFDIR)/$$p.dist; \
	  else \
	  echo " $(INSTALL_DATA) $$p $(DESTDIR)$(RMS_SYSCONFDIR)/$$p"; \
	  $(INSTALL_DATA) $$p $(DESTDIR)$(RMS_SYSCONFDIR)/$$p; \
	  fi; \
	done
	
RMS_SYSCONFDIR = $(sysconfdir)/@PACKAGE@

EXTRA_DIST = $(etcfiles)
