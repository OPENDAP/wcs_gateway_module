
# Automake file for wcs-handler
#
# 5/17/2005 jhrg
#
# $Id: Makefile.am 12972 2006-01-05 15:41:07Z pwest $

AUTOMAKE_OPTIONS = foreign check-news

ACLOCAL_AMFLAGS = -I conf

SUBDIRS = unit-tests testsuite
DIST_SUBDIRS = unit-tests testsuite

lib_besdir=$(libdir)/bes
lib_bes_LTLIBRARIES = libwcs_module.la
dist_bin_SCRIPTS = bes-wcs-data.sh

WCS_SRCS:=

WCS_HDRS:=

BES_SRCS = \
		WCSModule.cc WCSRequest.cc WCSContainerStorage.cc	\
		WCSContainer.cc WCSUtils.cc WCSError.cc			\
		WCSRequestHandler.cc

BES_HDRS = \
		WCSModule.h WCSRequest.h WCSContainerStorage.h		\
		WCSContainer.h WCSUtils.h WCSError.h			\
		WCSRequestHandler.h

libwcs_module_la_SOURCES = $(WCS_SRCS) $(BES_SRCS) $(WCS_HDRS) $(BES_HDRS)
libwcs_module_la_CPPFLAGS = $(BES_CPPFLAGS)
libwcs_module_la_LDFLAGS = -avoid-version -module 
libwcs_module_la_LIBADD = $(BES_LIBS)

EXTRA_PROGRAMS = 

EXTRA_DIST = COPYRIGHT_URI COPYING wcs_module.spec bes-wcs-data.sh.in

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

# Build OS/X Packages.

PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker*

UNIV_ARCH=-arch ppc -arch i386
MAC_SYSROOT=Developer/SDKs/MacOSX10.4u.sdk

PKG_CONF_FLAGS=

clean-pkg:
	-rm -rf mac_osx @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@

pkg-build:
	./configure --prefix=/usr --disable-dependency-tracking $(PKG_CONF_FLAGS)
	make clean all
	DESTDIR=${PWD}/mac_osx make install

pkg-main:
	./OSX_Resources/update_mac_package_contents.pl README "configure.ac" "mac_osx/usr"
	${PACKAGEMAKER} -build -p @PACKAGE@-@PACKAGE_VERSION@.pkg \
		-f mac_osx/usr \
		-ds -r OSX_Resources \
		-i OSX_Resources/Info.plist \
		-d OSX_Resources/Description.plist
	-rm -rf mac_osx

pkg-dmg:
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@.dmg
	mkdir @PACKAGE@-@PACKAGE_VERSION@
	mv @PACKAGE@-@PACKAGE_VERSION@.pkg @PACKAGE@-@PACKAGE_VERSION@
	cp README NEWS @PACKAGE@-@PACKAGE_VERSION@
	dropdmg -i --sanitize-for-servers --format zlib @PACKAGE@-@PACKAGE_VERSION@
	-rm -rf @PACKAGE@-@PACKAGE_VERSION@

pkg: clean-pkg pkg-build pkg-main pkg-dmg

