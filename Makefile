INSTALL_BIN=/usr/local/bin

all:
	cd src ; make all

install:
	cp -f bin/* ${INSTALL_BIN}/.

clean:
	cd src ; make clean

distclean:
	make clean
	rm -f *~
	cd src ; make distclean
