SHELL = /bin/sh
# PLATFORM should be set to "linux" or "solaris"
# (default: "linux")
PLATFORM = linux
CC = g++
#DEBUG = -g
OPT = -O3 -funroll-loops
LARGEFILE = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CXXFLAGS = -Wall ${DEBUG} ${OPT} ${INCLUDE} ${LARGEFILE}
CLEAN = *.o core *.core
DISTCLEAN = *~
RM = rm -f
BINDIR = ../bin
LIBDIR = ../lib
#XERVER = 1_7_0
#XERLIB = xerces-c${XERVER}
#XERCES_DIR = xerces-c-src${XERVER}
#XERCES_PKG = xerces-c-src${XERVER}.tar.gz
#XERCES_LIB = lib${XERLIB}.so
#INCLUDE = -Iinclude -Iengine -Idclass -Ietc/${XERCES_DIR}/src
INCLUDE = -Iinclude -Iengine -Idclass

LIBS=-lxerces-c

all: ${BINDIR}/afindex ${BINDIR}/afsearch ${BINDIR}/afadmin ${BINDIR}/affetch
	@echo ""
	@echo "Successful completion."
	@echo ""
#	@echo "******************************************************************"
#	@echo "The LD_LIBRARY_PATH environment variable should be set as follows:"
#	@echo ""
#	@echo "bash:"
#	@cd ../lib ; \
#	echo " " export LD_LIBRARY_PATH=`pwd`:"$$"LD_LIBRARY_PATH
#	@echo ""
#	@echo "csh:"
#	@cd ../lib ; \
#	echo " " setenv LD_LIBRARY_PATH `pwd`:"$$"LD_LIBRARY_PATH
#	@echo ""
#	@echo "******************************************************************"
#	@echo ""

${BINDIR}/afindex: ${LIBDIR}/${XERCES_LIB} ${LIBDIR}/libaf.a tools/afindex.o
	${CC} -o ${BINDIR}/afindex tools/afindex.o -L${LIBDIR} -laf -lm ${LIBS}

${BINDIR}/afsearch: ${LIBDIR}/${XERCES_LIB} ${LIBDIR}/libaf.a tools/afsearch.o
	${CC} -o ${BINDIR}/afsearch tools/afsearch.o -L${LIBDIR} -laf -lm ${LIBS}

${BINDIR}/afadmin: ${LIBDIR}/${XERCES_LIB} ${LIBDIR}/libaf.a tools/afadmin.o
	${CC} -o ${BINDIR}/afadmin tools/afadmin.o -L${LIBDIR} -laf -lm ${LIBS}

${BINDIR}/affetch: tools/affetch.o
	${CC} -o ${BINDIR}/affetch tools/affetch.o

#${LIBDIR}/${XERCES_LIB}: etc/${XERCES_DIR}
#	cd etc/${XERCES_DIR}/src/xercesc ; \
#	autoconf ; \
#	XERCESCROOT=`pwd`/../.. /bin/sh runConfigure -p${PLATFORM} -cgcc -xg++ -minmem -nsocket -tnative -rpthread ; \
#	XERCESCROOT=`pwd`/../.. gmake ; \
#	cp ../../lib/${XERCES_LIB} ../../../../../lib/.

#etc/${XERCES_DIR}:
#	cd etc ; \
#	if [ ! -d ${XERCES_DIR} ] ; \
#		then tar xfz ${XERCES_PKG} ; \
#        fi ; \

SRC = include/af.h \
		include/af_auto.h \
		engine/engine.h \
		engine/engine.cc \
		dclass/text.h \
		dclass/text.cc \
		dclass/xml.h \
		dclass/xml.cc \
		dclass/xml_test.h \
		dclass/xml_test.cc \
		dclass/syr1.h \
		dclass/syr1.cc \
		engine/docbuf.cc \
		engine/fdef.cc \
		engine/index.cc \
		engine/lock.cc \
		engine/search.cc \
		engine/util.cc \
		engine/admin.cc \
		engine/open.cc \
		engine/log.cc \
		engine/defs.h

OBJ = engine/engine.o \
		dclass/text.o \
		dclass/xml.o \
		dclass/xml_test.o \
		dclass/syr1.o

${LIBDIR}/libaf.a: ${OBJ}
	ar rs ${LIBDIR}/libaf.a ${OBJ}

engine/engine.o: ${SRC}

dclass/text.o: ${SRC}

dclass/xml.o: ${SRC}

dclass/xml_test.o: ${SRC}

dclass/syr1.o: ${SRC}

include/af_auto.h: autoconf/configure ../VERSION include/af_auto.h.in
	cd autoconf ; CC=${CC} ./configure
	echo "#define ETYMON_AF_VERSION \"`cat ../VERSION`\"" >> include/af_auto.h

autoconf/configure: autoconf/configure.in
	cd autoconf ; autoconf

clean:
	${RM} ${CLEAN}
	cd autoconf ; ${RM} ${CLEAN} config.* ; ${RM} -r autom4te.cache
	cd include ; ${RM} ${CLEAN} af_auto.h
	cd engine ; ${RM} ${CLEAN}
	cd dclass ; ${RM} ${CLEAN}
	cd tools ; ${RM} ${CLEAN}
	cd ${LIBDIR} ; ${RM} libaf.a ${XERCES_LIB}
	cd ${BINDIR} ; ${RM} afindex afsearch afadmin affetch

distclean:
	make clean
	${RM} ${DISTCLEAN}
	cd autoconf ; ${RM} ${DISTCLEAN}
	cd include ; ${RM} ${DISTCLEAN}
	cd engine ; ${RM} ${DISTCLEAN}
	cd dclass ; ${RM} ${DISTCLEAN}
	cd tools ; ${RM} ${DISTCLEAN}
