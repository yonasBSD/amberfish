SHELL =		/bin/sh

all:
	@if [ ! -f src/Makefile ] ; then \
		echo '*** Makefile not found in src/.  Did you forget to ./configure ? ***' ; \
	else \
		${MAKE} build ; \
	fi

build:
	cd src ; ${MAKE} all
	cd doc ; ${MAKE} all

install:
	cd src ; ${MAKE} ${MAKEFLAGS} install
	cd doc ; ${MAKE} ${MAKEFLAGS} install

uninstall:
	cd src ; ${MAKE} ${MAKEFLAGS} uninstall
	cd doc ; ${MAKE} ${MAKEFLAGS} uninstall

clean:
	rm -fr autom4te.cache
	rm -f config.log config.status
	cd src ; ${MAKE} clean
	cd doc ; ${MAKE} clean

distclean:
	${MAKE} clean
	cd src ; ${MAKE} distclean
	cd doc ; ${MAKE} distclean
	rm -f src/Makefile src/config.h src/version.h \
		doc/Makefile doc/version.texi
	rm -f *~ *#
