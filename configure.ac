# Process this file with autoconf to produce a configure script.
#

# Prelude
AC_INIT(indexf,4.1.3,[cardiel@fis.ucm.es])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/indexf.cpp])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs
AC_PROG_CXX

AC_ARG_WITH([pgplot],
            [AS_HELP_STRING([--with-pgplot],
              [use pgplot (default is yes)])],
              [],
              [])

AM_CONDITIONAL([WITHPGPLOT], [test x$with_pgplot != xno])

AS_IF([test "x$with_pgplot" != xno],
    [
      AC_MSG_CHECKING([uname -s for detecting host operating system])
      OSNAME=`uname -s`
      case "$OSNAME" in
         Darwin*)
           AC_MSG_RESULT(Darwin)
           LDFLAGS="$LDFLAGS -L/opt/local/lib"
           CXXFLAGS="$CXXFLAGS -I/opt/local/include"
           CPPFLAGS="$CXXFLAGS -I/opt/local/include"
          ;;
          Linux*)
            AC_MSG_RESULT(Linux)
            LDFLAGS="$LDFLAGS -L/usr/local/pgplot"
            CXXFLAGS="$CXXFLAGS -I/usr/local/pgplot"
            CPPFLAGS="$CXXFLAGS -I/usr/local/pgplot"
          ;;
       esac
     AC_PROG_F77 # this is required to know which is $F77 (see Checks for libraries)
     AC_CHECK_LIB([X11], main,,AC_MSG_ERROR(X11 library not found))
     AC_CHECK_LIB([pgplot], main,,AC_MSG_ERROR(pgplot library not found))
     AC_CHECK_HEADER([cpgplot.h],,AC_MSG_ERROR(cpgplot header not found))
     AC_CHECK_LIB([cpgplot], main,,AC_MSG_ERROR(cpgplot library not found))
], [])

PKG_CHECK_MODULES([CFITSIO], [cfitsio])
case "$OSNAME" in
  Linux*)
  # Note: -lgfortran (or -lg77) is required to link the executable file
  AC_CHECK_LIB([$F77], main,,AC_MSG_ERROR($F77 library not found))
  ;;
esac
# Define in LIBS all the previous libraries
LIBS="$CFITSIO_LIBS $LIBS"
AC_MSG_RESULT(LIBS set to... $LIBS)

# Definitions
DEFAULT_AUXDIR="/usr/local/share/indexf"
AUXDIR=${AUXDIR="$DEFAULT_AUXDIR"}
AC_ARG_VAR(AUXDIR,[Directory to install auxiliary indexf files])

# Output files
AC_CONFIG_FILES([Makefile
                 src/installdir.h
                 src/version.h
                 src/Makefile
                 auxdir/Makefile])
AC_OUTPUT
