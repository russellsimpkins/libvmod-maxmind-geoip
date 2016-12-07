AC_PREREQ(2.59)
AC_COPYRIGHT([Copyright (c) 2016 The New York Times])
AC_INIT([libvmod-geo], [trunk])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/vmod_geo.vcc)
AC_CONFIG_HEADERS(config.h)

AC_CANONICAL_SYSTEM
AC_LANG(C)

AM_INIT_AUTOMAKE([foreign])

AC_GNU_SOURCE
AC_PROG_CC
AC_PROG_CC_STDC
if test "x$ac_cv_prog_cc_c99" = xno; then
	AC_MSG_ERROR([Could not find a C99 compatible compiler])
fi
AC_PROG_CPP

AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET

# Check for rst utilities
AC_CHECK_PROGS(RST2MAN, [rst2man rst2man.py], "no")
if test "x$RST2MAN" = "xno"; then
	AC_MSG_WARN([rst2man not found - not building man pages])
fi
AM_CONDITIONAL(HAVE_RST2MAN, [test "x$RST2MAN" != "xno"])

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([sys/stdlib.h])

# Check for python
AC_CHECK_PROGS(PYTHON, [python3 python3.1 python3.2 python2.7 python2.6 python2.5 python2 python], [AC_MSG_ERROR([Python is needed to build this vmod, please install python.])])


AC_CHECK_FILE([/usr/share/aclocal/varnish.m4],
	[],
	[AC_MSG_FAILURE(["Varnish 4.1 is not installed. Please install varnish4.1 to build this code.])]
)

# Check for libmaxminddb
AC_ARG_WITH([maxminddb],
    [AS_HELP_STRING([--with-maxminddb],
	[the directory where the maxminddb SDK is installed in])],
    [
        CPPFLAGS="$CPPFLAGS -I${withval}/include"
        LDFLAGS="$LDFLAGS -L${withval}/lib"
    ],
    [:]
)
AC_ARG_WITH([maxminddbfile],
    [AS_HELP_STRING([--with-maxminddbfile],
	[the maximind city db file e.g. /mnt/mmdb/GeoLite2-City.mmdb])],
    [
        AC_SUBST(MAX_CITY_DB,[${withval}])
    ],
    [:]
)
AC_CHECK_LIB([maxminddb], [MMDB_get_entry_data_list],
    [AC_CHECK_HEADER([maxminddb.h],
        [],
        [AC_MSG_ERROR([Failed to find the maxminddb SDK (1). Specify --with-maxminddb=PATH])]
    )],
    [AC_MSG_ERROR([Failed to find the maxminddb SDK (2). Specify --with-maxminddb=PATH])]
)
LIBS="$LIBS -lmaxminddb"
# backwards compat with older pkg-config
# - pull in AC_DEFUN from pkg.m4
m4_ifndef([PKG_CHECK_VAR], [
# PKG_CHECK_VAR(VARIABLE, MODULE, CONFIG-VARIABLE,
# [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# -------------------------------------------
# Retrieves the value of the pkg-config variable for the given module.
AC_DEFUN([PKG_CHECK_VAR],
[AC_REQUIRE([PKG_PROG_PKG_CONFIG])dnl
AC_ARG_VAR([$1], [value of $3 for $2, overriding pkg-config])dnl

_PKG_CONFIG([$1], [variable="][$3]["], [$2])
AS_VAR_COPY([$1], [pkg_cv_][$1])

AS_VAR_IF([$1], [""], [$5], [$4])dnl
])# PKG_CHECK_VAR
])
PKG_CHECK_MODULES([libvarnishapi], [varnishapi])
PKG_CHECK_VAR([LIBVARNISHAPI_PREFIX], [varnishapi], [prefix])
PKG_CHECK_VAR([LIBVARNISHAPI_DATAROOTDIR], [varnishapi], [datarootdir])
PKG_CHECK_VAR([LIBVARNISHAPI_BINDIR], [varnishapi], [bindir])
PKG_CHECK_VAR([LIBVARNISHAPI_SBINDIR], [varnishapi], [sbindir])
AC_SUBST([LIBVARNISHAPI_DATAROOTDIR])

# Varnish include files tree
VARNISH_VMOD_INCLUDES
VARNISH_VMOD_DIR
VARNISH_VMODTOOL
# inherit the prefix from Varnish.
# acessing ac_ variable because AC_PREFIX_DEFAULT acts too early
ac_default_prefix=$LIBVARNISHAPI_PREFIX

AC_PATH_PROG([VARNISHTEST], [varnishtest], [],
    [$LIBVARNISHAPI_BINDIR:$LIBVARNISHAPI_SBINDIR:$PATH])
AC_PATH_PROG([VARNISHD], [varnishd], [],
    [$LIBVARNISHAPI_SBINDIR:$LIBVARNISHAPI_BINDIR:$PATH])

# vmod installation dir
AC_ARG_VAR([VMODDIR], [vmod installation directory @<:@LIBDIR/varnish/vmods@:>@])
if test "x$VMODDIR" = x; then
	VMODDIR=`pkg-config --variable=vmoddir varnishapi`
	if test "x$VMODDIR" = x; then
		AC_MSG_FAILURE([Can't determine vmod installation directory through pkgconfig])
	fi
fi

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	tests/Makefile
    src/tests/Makefile
])
AC_OUTPUT
