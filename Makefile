SHELL =		/bin/sh
PREFIX =	/usr/local
BIN =		${PREFIX}/bin

all:
	@if [ ! -f src/Makefile ] ; then \
		echo '*** Makefile not found in src/.  Did you forget to ./configure ? ***' ; \
	else \
		make build ; \
	fi

build:
	cd src ; make all
	cd doc ; make all

install:
	cd src ; make install
	cd doc ; make install

uninstall:
	cd src ; make uninstall

clean:
	rm -fr autom4te.cache
	rm -f config.log config.status
	cd src ; make clean
	cd doc ; make clean

distclean:
	make clean
	cd src ; make distclean
	cd doc ; make distclean
	rm -f src/Makefile src/config.h src/version.h \
		doc/Makefile doc/version.texi
	rm -f *~ *#
