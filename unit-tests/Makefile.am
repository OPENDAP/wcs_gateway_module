# Tests

AUTOMAKE_OPTIONS = foreign

# Headers in 'tests' are used by the arrayT unit tests.

AM_CPPFLAGS = $(BES_CPPFLAGS)
AM_CXXFLAGS = 

# These are not used by automake but are often useful for certain types of
# debugging. Set CXXFLAGS to this in the nightly build using export ...
CXXFLAGS_DEBUG = -g3 -O0 -fno-defer-pop -Wall -W -Wcast-align -Werror
TEST_COV_FLAGS = -ftest-coverage -fprofile-arcs

# This determines what gets built by make check
check_PROGRAMS = $(UNIT_TESTS)

# This determines what gets run by 'make check.'
TESTS = $(UNIT_TESTS)

DIRS_EXTRA = 

EXTRA_DIST = $(DIRS_EXTRA) 

CLEANFILES = 

############################################################################
# Unit Tests
#

UNIT_TESTS = validateUrlT


validateUrlT_SOURCES = validateUrlT.cc
validateUrlT_LDADD = -lcppunit ../libwcs_module.la

