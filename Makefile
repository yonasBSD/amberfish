INSTALL_BIN=/usr/local/bin

all:
	@if [ ! -f src/Makefile ] ; then \
		echo '*** Makefile not found in src/.  Did you forget to ./configure ? ***' ; \
	else \
		make build ; \
	fi

build:
	cd src ; make all

install:
	cp -f bin/* ${INSTALL_BIN}/.

clean:
	rm -fr autom4te.cache
	rm -f config.log config.status
	cd src ; make clean

distclean:
	make clean
	cd src ; make distclean
	rm -f src/Makefile src/include/version.h src/include/af_auto.h
	rm -f *~

changelog:
	rcs2log > ./ChangeLog
