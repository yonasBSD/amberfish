SHELL =		/bin/sh
PREFIX =	/usr/local
BIN =		${PREFIX}/bin

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
	cd src ; ${MAKE} install
	cd doc ; ${MAKE} install

uninstall:
	cd src ; ${MAKE} uninstall

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
