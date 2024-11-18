SHELL =		/bin/sh

all:
	@if [ ! -f src/backend/Makefile ] ; then \
		echo '*** Makefile not found in src/backend/.  Did you forget to ./configure ? ***' ; \
	else \
		${MAKE} build ; \
	fi

build:
	cd src/backend ; ${MAKE} all
#	cd src/interface ; ${MAKE} all
	cd doc ; ${MAKE} all

html:
	cd doc ; ${MAKE} html

strip:
	cd src/backend ; ${MAKE} ${MAKEFLAGS} strip
#	cd src/interface ; ${MAKE} ${MAKEFLAGS} strip

install:
	cd src/backend ; ${MAKE} ${MAKEFLAGS} install
#	cd src/interface ; ${MAKE} ${MAKEFLAGS} install
	cd doc ; ${MAKE} ${MAKEFLAGS} install

uninstall:
	cd src/backend ; ${MAKE} ${MAKEFLAGS} uninstall
#	cd src/interface ; ${MAKE} ${MAKEFLAGS} uninstall
	cd doc ; ${MAKE} ${MAKEFLAGS} uninstall

clean:
	rm -fr autom4te.cache
	rm -f config.log config.status
	cd src/backend ; ${MAKE} clean
	cd src/interface ; ${MAKE} clean
	cd doc ; ${MAKE} clean

distclean:
	${MAKE} clean
	cd src/backend ; ${MAKE} distclean
	cd src/interface ; ${MAKE} distclean
	cd doc ; ${MAKE} distclean
	cd src/backend ; rm -f Makefile config.h version.h
	cd src/interface ; rm -f Makefile config.h version.h
	cd doc ; rm -f doc/Makefile doc/version.adoc
	rm -f configure *~ *#
