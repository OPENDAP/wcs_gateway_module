# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
# Configure script for OPeNDAP WCS data handler

AC_PREREQ(2.59)
AC_INIT(wcs_gateway_module, 1.1.0, opendap-tech@opendap.org)
AC_CONFIG_AUX_DIR([conf])
AC_CONFIG_MACRO_DIR([conf])

AC_CONFIG_SRCDIR([WCSGatewayModule.h])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

AC_CANONICAL_HOST
AC_SUBST(host)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_CHECK_FUNCS([strchr])

dnl Checks for specific libraries
AC_CHECK_LIBDAP([3.7.10],
 [
  LIBS="$LIBS $DAP_CLIENT_LIBS"
  CPPFLAGS="$CPPFLAGS $DAP_CFLAGS"
 ],
 [ AC_MSG_ERROR([Cannot find libdap])
])
  
AC_CHECK_BES([3.1.0],
 [
 ],
 [ AC_MSG_ERROR([Cannot find bes])
])
  

AC_MSG_CHECKING([for libxml2])
if xml2-config --version > /dev/null 2>&1
then
	version_libxml2=`xml2-config --version`
	version_M=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	version_m=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	version_m_m=`echo $version_libxml2 | sed 's@^\([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\3@'`
	
	dnl Test for several different versions of libxml2. We can use 2.5.7 
	dnl or newer, but part of the SAX interface changes depending on the
	dnl version.
	
	dnl First test the minimum
	if test $version_M -lt 2 || test $version_m -lt 5 || test $version_m_m -lt 7
	then
		AC_MSG_ERROR([must have libxml2 2.5.7 or greater, found $version_libxml2])
	fi
	
	dnl Sort out the particular variant
	if test $version_M -eq 2 && test $version_m -eq 5 && test $version_m_m -ge 10
	then
		AC_DEFINE(LIBXML2_5_10, [1], [define if you have xml2 2.5.10 or greater])
	fi
	if test $version_M -eq 2 && test $version_m -eq 6 && test $version_m_m -ge 16
	then
		AC_DEFINE(LIBXML2_5_10, [1], [define if you have xml2 2.5.10 or greater])
		AC_DEFINE(LIBXML2_6_16, [1], [define if you have xml2 2.6.16 or greater])
	fi

	LIBS="$LIBS `xml2-config --libs`"
	CPPFLAGS="$CPPFLAGS `xml2-config --cflags`"
	AC_MSG_RESULT([yes])
else
	AC_MSG_ERROR([I could not find libxml2])
fi

dnl I wrote these checks because we need the *-config scripts to build, so 
dnl the AC_CHECK_LIB macro is not needed.
AC_MSG_CHECKING([for libcurl])
if curl-config --version > /dev/null 2>&1
then
	version_libcurl=`curl-config --version`
	version_major=`echo $version_libcurl | sed 's@^libcurl \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\1@'`
	version_minor=`echo $version_libcurl | sed 's@^libcurl \([[0-9]]\)*\.\([[0-9]]*\)\.\([[0-9]]*\)$@\2@'`
	if test $version_major -lt 7 || test $version_minor -lt 10 || test $version_minor -lt 6
	then
		AC_MSG_ERROR([must have libcurl 7.10.6 or greater, found $version_libcurl])
	fi

	LIBS="$LIBS `curl-config --libs`"
	CPPFLAGS="$CPPFLAGS `curl-config --cflags`"
	AC_MSG_RESULT([yes])
else
	AC_MSG_ERROR([I could not find libcurl])
fi

AM_PATH_CPPUNIT(1.12.0,
	[AM_CONDITIONAL([CPPUNIT], [true])],
	[AM_CONDITIONAL([CPPUNIT], [false])])

AC_CONFIG_FILES([Makefile unit-tests/Makefile testsuite/Makefile])

AC_OUTPUT

