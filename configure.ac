AC_INIT

USER_CPPFLAGS=$CPPFLAGS
USER_LDFLAGS=$LDFLAGS
CPPFLAGS="-I/usr/local/include $CPPFLAGS"
LDFLAGS="-L/usr/local/lib $LDFLAGS"

af_version=`git describe --tags --always`

AC_PROG_CC
AC_PROG_CXX

AC_CHECK_FUNC(getopt_long, , AC_CHECK_LIB(iberty, getopt_long))

AC_CHECK_SIZEOF(short int, 0)
AC_CHECK_SIZEOF(int, 0)
AC_CHECK_SIZEOF(long int, 0)

xml=yes
AC_ARG_ENABLE(xml, [  --disable-xml           disable linking with Xerces and XML support], [
	if test "$enableval" = "no"; then
		xml=no
	fi
] )
AC_MSG_CHECKING(whether xml is enabled)
if test "$xml" = "yes"; then
	AC_MSG_RESULT(yes)
	AC_CHECK_LIB(xerces-c, main, [
			xmllib=yes
			LIBS="-lxerces-c $LIBS"
			AC_DEFINE(HAVE_LIBXERCES_C)
		], 
		xmllib=no)
	AC_CHECK_HEADERS([xercesc/util/XercesVersion.hpp], xmlinc=yes, xmlinc=no)
else
	AC_MSG_RESULT(no)
fi
xmlon=no
if test "$xmllib" = "yes"; then
	if test "$xmlinc" = "yes"; then
		xmlon=yes
		AC_DEFINE(ETYMON_AF_XML)
	fi
fi

stemmer=no
rm -f src/backend/stem.d
AC_MSG_CHECKING(for stemmer)
if test -f src/backend/porter.cc; then
	stemmer=yes
	AC_DEFINE(HAVE_STEMMER)
fi
AC_MSG_RESULT($stemmer)

AC_MSG_NOTICE(creating src/backend/version.h)
echo "#define AF_VERSION \"$af_version\"" > src/backend/version.h
AC_MSG_NOTICE(creating doc/version.adoc)
echo "$af_version" > doc/version.adoc

AC_CONFIG_HEADERS([src/backend/config.h])
AC_CONFIG_FILES(src/backend/Makefile)
AC_CONFIG_FILES(src/interface/Makefile)
AC_CONFIG_FILES(doc/Makefile)

AC_SUBST(BIN, $bin)

AC_OUTPUT

echo ""
echo "User options"
echo "------------"
echo "Install prefix                : $prefix"
echo "CPPFLAGS                      : $USER_CPPFLAGS"
echo "LDFLAGS                       : $USER_LDFLAGS"
echo "Enable XML                    : $xml"
echo ""
echo "Configure results"
echo "-----------------"
echo "Amberfish version             : $af_version"
echo "C compiler                    : $CC"
echo "C++ compiler                  : $CXX"
if test "$xml" = "yes"; then
	echo "Xerces-C library found        : $xmllib"
	echo "Xerces-C headers found        : $xmlinc"
fi
echo "XML support enabled           : $xmlon"
echo "Stemmer enabled               : $stemmer"
echo ""
