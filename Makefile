PREFIX = '/usr'
DESTDIR = ''
CFLAGS = -march=native -mtune=native -O2 -ansi -pedantic -Wall -std=c11


PROGRAM_NAME := $(shell grep ^PROGRAM_NAME INFO | cut -d= -f2)
EXECUTABLE_NAME := $(shell grep ^EXECUTABLE_NAME INFO | cut -d= -f2)
VERSION := $(shell grep ^VERSION INFO | cut -d= -f2)

ARCHPKG = $(EXECUTABLE_NAME)-$(VERSION)-1-any.pkg.tar.xz

src/$(EXECUTABLE_NAME):

install: install_executables install_services
clean: arch_clean
	rm -f src/$(EXECUTABLE_NAME)

install_executables:
	install -Dm 755 src/$(EXECUTABLE_NAME) $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE_NAME)

install_services:
	install -Dm 644 src/$(EXECUTABLE_NAME).service $(DESTDIR)/lib/systemd/system/$(EXECUTABLE_NAME).service

uninstall:
	rm -f $(PREFIX)/bin/$(EXECUTABLE_NAME)
	rm -f /lib/systemd/system/$(EXECUTABLE_NAME).service

arch_install: install_executables arch_install_services
arch_install_services:
	install -Dm644 src/$(EXECUTABLE_NAME).service $(DESTDIR)$(PREFIX)/lib/systemd/system/$(EXECUTABLE_NAME).service

arch_pkg: $(ARCHPKG)
$(ARCHPKG): PKGBUILD ChangeLog
	makepkg -fd
	@echo
	@echo Package done!
	@echo You can install it as root with:
	@echo pacman -U $@

arch_clean:
	rm -rf pkg
	rm -f $(ARCHPKG)

.PHONY: clean arch_pkg arch_clean
