SHELL =		/bin/sh

all:
	cd src/backend ; ${MAKE} ${MFLAGS} all
#	cd src/interface ; ${MAKE} ${MFLAGS} all
	cd doc ; ${MAKE} ${MFLAGS} all

html:
	cd doc ; ${MAKE} ${MFLAGS} html

strip:
	cd src/backend ; ${MAKE} ${MFLAGS} strip
#	cd src/interface ; ${MAKE} ${MFLAGS} strip

install:
	cd src/backend ; ${MAKE} ${MFLAGS} install
#	cd src/interface ; ${MAKE} ${MFLAGS} install
	cd doc ; ${MAKE} ${MFLAGS} install

uninstall:
	cd src/backend ; ${MAKE} ${MFLAGS} uninstall
#	cd src/interface ; ${MAKE} ${MFLAGS} uninstall
	cd doc ; ${MAKE} ${MFLAGS} uninstall

clean:
	rm -fr autom4te.cache
	rm -f config.log config.status
	cd src/backend ; ${MAKE} ${MFLAGS} clean
	cd src/interface ; ${MAKE} ${MFLAGS} clean
	cd doc ; ${MAKE} ${MFLAGS} clean

distclean:
	${MAKE} ${MFLAGS} clean
	cd src/backend ; ${MAKE} ${MFLAGS} distclean
	cd src/interface ; ${MAKE} ${MFLAGS} distclean
	cd doc ; ${MAKE} ${MFLAGS} distclean
	cd src/backend ; rm -f Makefile config.h version.h
	cd src/interface ; rm -f Makefile config.h version.h
	cd doc ; rm -f doc/Makefile doc/version.adoc
	rm -f *~ *#
