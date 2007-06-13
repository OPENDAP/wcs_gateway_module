
# Automake file for wcs-handler
#
# 5/17/2005 jhrg
#
# $Id: Makefile.am 12972 2006-01-05 15:41:07Z pwest $

AUTOMAKE_OPTIONS = foreign check-news

ACLOCAL_AMFLAGS = -I conf

lib_LTLIBRARIES = libwcs_module.la

bin_PROGRAMS = 

WCS_SRCS:=

WCS_HDRS:=

BES_SRCS = \
		WCSModule.cc WCSRequestHandler.cc WCSFile.cc	\
		WCSRequest.cc WCSCache.cc WCSContainerStorage.cc \
		WCSContainer.cc WCSUtils.cc

BES_HDRS = \
		WCSModule.h WCSRequestHandler.h WCSFile.h	\
		WCSRequest.h WCSCache.h WCSContainerStorage.h	\
		WCSContainer.h WCSUtils.h WCSResponseNames.h config_wcs.h

libwcs_module_la_SOURCES = $(WCS_SRCS) $(BES_SRCS) $(WCS_HDRS) $(BES_HDRS)
libwcs_module_la_CPPFLAGS = $(BES_CPPFLAGS)
libwcs_module_la_LDFLAGS = -avoid-version -module 
libwcs_module_la_LIBADD = $(BES_LIBS)

EXTRA_PROGRAMS = 

EXTRA_DIST = COPYRIGHT COPYING wcs_handler.spec

CLEANFILES = *~

###########################################################################

# Build linux RPMs

srpm:
	rpmbuild -ts wcs_module-@PACKAGE_VERSION@.tar.gz

rpm:
	rpmbuild -tb wcs_module-@PACKAGE_VERSION@.tar.gz
