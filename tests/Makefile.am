AM_CFLAGS = -Wall -D_GNU_SOURCE -DMAX_CITY_DB=\"$(MAX_CITY_DB)\"
AM_CPPFLAGS = -I$(top_srcdir)/src

tests.c: .FORCE
	ruby $(top_srcdir)/auto/generate_test_runner.rb tests.c test_Runner.c

.FORCE:

bin_PROGRAMS = testrunner test_ips
testrunner_LDFLAGS = -lmaxminddb -lm
test_ips_LDFLAGS = -lmaxminddb -lm
testrunner_SOURCES = \
	tests.c \
	unity.c \
	../src/geo_functions.c \
	test_Runner.c
test_ips_SOURCES = \
	../src/geo_functions.c \
	test_ips.c
