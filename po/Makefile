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

.PHONY: all clean install uninstall

all: $(MOFILES)

%.mo: %.po
	$(MSGFMT) --check -o $@ $<

clean:
	$(RM) $(MOFILES)

install: all
	@for lang in $(LINGUAS); do \
		$(INSTALL) -d "$(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES"; \
		$(INSTALL) -m 644 "$$lang.mo" "$(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES/$(DOMAIN).mo"; \
	done

uninstall:
	@for lang in $(LINGUAS); do \
		$(RM) "$(DESTDIR)$(LOCALEDIR)/$$lang/LC_MESSAGES/$(DOMAIN).mo"; \
	done
