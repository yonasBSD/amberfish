SHELL =		/bin/sh

all:
	@if [ ! -f src/backend/Makefile ] ; then \
		echo '*** Makefile not found in src/backend/.  Did you forget to ./configure ? ***' ; \
	else \
		${MAKE} build ; \
	fi

build:
	cd src/backend ; ${MAKE} all
#	cd src/thump ; ${MAKE} all
	cd doc ; ${MAKE} all

html:
	cd doc ; ${MAKE} html

pdf:
	cd doc ; ${MAKE} pdf

strip:
	cd src/backend ; ${MAKE} ${MAKEFLAGS} strip
#	cd src/thump ; ${MAKE} ${MAKEFLAGS} strip

install:
	cd src/backend ; ${MAKE} ${MAKEFLAGS} install
#	cd src/thump ; ${MAKE} ${MAKEFLAGS} install
	cd doc ; ${MAKE} ${MAKEFLAGS} install

uninstall:
	cd src/backend ; ${MAKE} ${MAKEFLAGS} uninstall
#	cd src/thump ; ${MAKE} ${MAKEFLAGS} uninstall
	cd doc ; ${MAKE} ${MAKEFLAGS} uninstall

clean:
	rm -fr autom4te.cache
	rm -f config.log config.status
	cd src/backend ; ${MAKE} clean
#	cd src/thump ; ${MAKE} clean
	cd doc ; ${MAKE} clean

distclean:
	${MAKE} clean
	cd src/backend ; ${MAKE} distclean
#	cd src/thump ; ${MAKE} distclean
	cd doc ; ${MAKE} distclean
	cd src/backend ; rm -f Makefile config.h version.h
#	cd src/thump ; rm -f Makefile config.h version.h
	cd doc ; rm -f doc/Makefile doc/version.texi
	rm -f configure *~ *#
