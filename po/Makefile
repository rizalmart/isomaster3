# Gettext catalogues for ISO Master.
# LOCALEDIR and DESTDIR are supplied by the top-level Makefile.

MSGFMT  ?= msgfmt
INSTALL ?= install
RM      ?= rm -f

LOCALEDIR ?= /usr/local/share/locale
DOMAIN = isomaster

POFILES = $(wildcard *.po)
LINGUAS = $(POFILES:.po=)
MOFILES = $(POFILES:.po=.mo)
STAGEDIR = ../locale

.PHONY: all clean install uninstall

all: $(MOFILES) stage

%.mo: %.po
	$(MSGFMT) --check -o $@ $<

# Stage catalogs next to the binary so the program works from the build tree
# without 'make install' (the binary falls back to <exedir>/locale).
stage: $(MOFILES)
	@for lang in $(LINGUAS); do \
		$(INSTALL) -d "$(STAGEDIR)/$$lang/LC_MESSAGES"; \
		$(INSTALL) -m 644 "$$lang.mo" "$(STAGEDIR)/$$lang/LC_MESSAGES/$(DOMAIN).mo"; \
	done

clean:
	$(RM) $(MOFILES)
	$(RM) -r $(STAGEDIR)

install: all
	@for lang in $(LINGUAS); do \
		$(INSTALL) -d "$(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES"; \
		$(INSTALL) -m 644 "$$lang.mo" "$(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES/$(DOMAIN).mo"; \
	done

uninstall:
	@for lang in $(LINGUAS); do \
		$(RM) "$(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES/$(DOMAIN).mo"; \
	done
