
AUTOMAKE_OPTIONS = foreign dejagnu

DEJATOOL=besstandalone

AM_CPPFLAGS = -I$(top_srcdir)

# AM_CPPFLAGS =
# AM_CXXFLAGS =

# These are not used by automake but are often useful for certain types of
# debugging. The best way to use these is to run configure as:
#     export CXXFLAGS='...'; ./configure --disable-shared
# the --disable-shared is not required, but it seems to help with debuggers.
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

lib_besdir=$(libdir)/bes
lib_bes_LTLIBRARIES = libwcs_test_module.la

noinst_DATA = bes.conf

libwcs_test_module_la_SOURCES = $(SRCS) $(HDRS)
libwcs_test_module_la_CPPFLAGS = $(BES_CPPFLAGS)
libwcs_test_module_la_LDFLAGS = -avoid-version -module
libwcs_test_module_la_LIBADD = $(BES_DAP_LIBS)

CLEANFILES = *.log *.sum site.exp

EXTRA_DIST = wcs besstandalone.wcs common_tests.exp bes.conf.in

DISTCLEANFILES = bes.conf

bes.conf: bes.conf.in ../config.status
	sed -e "s%[@]pkgdatadir[@]%${pkgdatadir}%" \
		-e "s%[@]sysconfdir[@]%${sysconfdir}%" \
		-e "s%[@]libdir[@]%${libdir}%" \
		-e "s%[@]datadir[@]%${datadir}%" \
		-e "s%[@]bindir[@]%${bindir}%" $< > bes.conf

##############################################################################
#
# Sources and Headers for libwcs_test_module.

SRCS:=WCSTestModule.cc WCSTestTransmitter.cc

HDRS:=WCSTestModule.h WCSTestTransmitter.h
