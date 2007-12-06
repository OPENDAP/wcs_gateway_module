
# Automake file for wcs-handler
#
# 5/17/2005 jhrg
#
# $Id: Makefile.am 12972 2006-01-05 15:41:07Z pwest $

AUTOMAKE_OPTIONS = foreign check-news

ACLOCAL_AMFLAGS = -I conf

SUBDIRS = unit-tests
DIST_SUBDIRS = unit-tests

lib_besdir=$(libdir)/bes
lib_bes_LTLIBRARIES = libwcs_module.la
dist_bin_SCRIPTS = bes-wcs-data.sh

WCS_SRCS:=

WCS_HDRS:=

BES_SRCS = \
		WCSModule.cc WCSRequestHandler.cc WCSFile.cc	\
		WCSRequest.cc WCSCache.cc WCSContainerStorage.cc \
		WCSContainer.cc WCSUtils.cc WCSException.cc

BES_HDRS = \
		WCSModule.h WCSRequestHandler.h WCSFile.h	\
		WCSRequest.h WCSCache.h WCSContainerStorage.h	\
		WCSContainer.h WCSUtils.h WCSException.h	\
		WCSResponseNames.h config_wcs.h

libwcs_module_la_SOURCES = $(WCS_SRCS) $(BES_SRCS) $(WCS_HDRS) $(BES_HDRS)
libwcs_module_la_CPPFLAGS = $(BES_CPPFLAGS)
libwcs_module_la_LDFLAGS = -avoid-version -module 
libwcs_module_la_LIBADD = $(BES_LIBS)

EXTRA_PROGRAMS = 

EXTRA_DIST = COPYRIGHT COPYING wcs_handler.spec bes-wcs-data.sh

CLEANFILES = *~ bes-wcs-data.sh

do_subst = sed -e 's,[@]sysconfdir[@],$(sysconfdir),g' \
		   	   -e 's,[@]libdir[@],$(libdir),g' \
		   	   -e 's,[@]datadir[@],$(datadir),g'

bes-wcs-data.sh: bes-wcs-data.sh.in config.status
	$(do_subst) < $(srcdir)/bes-wcs-data.sh.in > bes-wcs-data.sh
	chmod +x bes-wcs-data.sh

# This makes sure that the handler modifies the bes which configure found
# even when distcheck is run.
bes-conf: bes-wcs-data.sh
	(bes_prefix=`bes-config --prefix` ; \
	if test "$$bes_prefix" = "/usr" ; \
	 then \
	    ./bes-wcs-data.sh /etc/bes/bes.conf /usr/lib/bes ;\
	else \
	    ./bes-wcs-data.sh $$bes_prefix/etc/bes/bes.conf $$bes_prefix/lib/bes ; \
	fi)

###########################################################################

# Fortify targets.

.PHONY: fortify
fortify:
	sourceanalyzer -b @PACKAGE@ $(MAKE)
	sourceanalyzer -b @PACKAGE@ -scan -f @PACKAGE@-@PACKAGE_VERSION@.fpr

# Use this to clean the fortify project.
.PHONY: fortifyclean
fortifyclean:
	sourceanalyzer -b @PACKAGE@ -clean

###########################################################################

# Build linux RPMs

srpm:
	rpmbuild -ts wcs_module-@PACKAGE_VERSION@.tar.gz

rpm:
	rpmbuild -tb wcs_module-@PACKAGE_VERSION@.tar.gz

###########################################################################
#
# Build OS/X Packages.

PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker*

pkg:
	rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg
	./configure --prefix=/usr $(PKG_CONFIGURE_FLAGS)
	make clean all
	DESTDIR=${PWD}/mac_osx make install
	${PACKAGEMAKER} -build -p @PACKAGE@-@PACKAGE_VERSION@.pkg -f mac_osx/usr -ds -r OSX_Resources -i OSX_Resources/Info.plist -d OSX_Resources/Description.plist

