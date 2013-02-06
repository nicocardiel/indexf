# Process this file with autoconf to produce a configure script.
#

# Prelude
AC_INIT(indexf,4.2.0,[cardiel@fis.ucm.es])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/indexf.cpp])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs
AC_PROG_CXX

AC_ARG_WITH([pgplot],
            [AS_HELP_STRING([--with-pgplot],
              [use pgplot (default is no)])],
              [],
              [with_pgplot=no])

AM_CONDITIONAL([WITHPGPLOT], [test x$with_pgplot != xno])

AS_IF([test "x$with_pgplot" != xno],
    [
      AC_PROG_F77 # required to know which is $F77
      AC_MSG_CHECKING([uname -s for detecting host operating system])
      OSNAME=`uname -s`
      case "$OSNAME" in
         Darwin*)
           AC_MSG_RESULT(Darwin)
           pgplot_ldflags="-L/opt/local/lib"
           pgplot_cflags="-I/opt/local/include"
           AC_PATH_X
          ;;
          Linux*)
            AC_MSG_RESULT(Linux)
            pgplot_ldflags="-L/usr/local/pgplot"
            pgplot_cflags="-I/usr/local/pgplot"
            AC_CHECK_LIB([$F77], main,,AC_MSG_ERROR($F77 library not found))
            AC_CHECK_LIB([X11], main,,AC_MSG_ERROR(X11 library not found))
          ;;
       esac
     AC_SUBST(PGPLOT_LDFLAGS, $pgplot_ldflags)
     AC_SUBST(PGPLOT_CFLAGS, $pgplot_cflags)
     AC_CHECK_LIB([pgplot], main, [],
       AC_MSG_ERROR(pgplot library not found), [$pgplot_ldflags])
     AC_CHECK_LIB([cpgplot], main, [],
       AC_MSG_ERROR(cpgplot library not found), [$pgplot_ldflags])
     old_CPPFLAGS=$CPPFLAGS
       CPPFLAGS="${pgplot_cflags} $CPPFLAGS" 
     AC_CHECK_HEADERS([cpgplot.h], [], 
       AC_MSG_ERROR(cpgplot header not found), [])
     CPPFLAGS=$old_CPPFLAGS 
], [])

PKG_CHECK_MODULES([CFITSIO], [cfitsio])

# Output files
AC_CONFIG_FILES([Makefile
                 src/version.h
                 src/Makefile
                 auxdir/Makefile])
AC_OUTPUT
