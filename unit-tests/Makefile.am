# Tests

AUTOMAKE_OPTIONS = foreign

# Headers in 'tests' are used by the arrayT unit tests.

AM_CPPFLAGS = -I$(top_srcdir) $(BES_CPPFLAGS)
AM_CXXFLAGS = 

# These are not used by automake but are often useful for certain types of
# debugging. Set CXXFLAGS to this in the nightly build using export ...
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

# This determines what gets built by make check
check_PROGRAMS = $(UNIT_TESTS)

nodist_include_HEADERS = test_config.h

# This determines what gets run by 'make check.'
TESTS = $(UNIT_TESTS)

DIRS_EXTRA = 

EXTRA_DIST = $(DIRS_EXTRA) test_config.h.in err.xml bes.conf

DISTCLEANFILES = test_config.h

CLEANFILES = *.dbg

test_config.h: test_config.h.in Makefile
	sed -e "s%[@]srcdir[@]%${srcdir}%" $< > test_config.h

clean-local:
	rm -rf wcs

check-local:
	rm -rf wcs

############################################################################
# Unit Tests
#

UNIT_TESTS = requestT validateUrlT convertTypeT xmlErrT

requestT_SOURCES = requestT.cc
requestT_LDADD = -lcppunit ../libwcs_module.la

validateUrlT_SOURCES = validateUrlT.cc
validateUrlT_LDADD = -lcppunit ../libwcs_module.la

convertTypeT_SOURCES = convertTypeT.cc
convertTypeT_LDADD = -lcppunit ../libwcs_module.la

xmlErrT_SOURCES = xmlErrT.cc
xmlErrT_LDADD = -lcppunit ../libwcs_module.la

