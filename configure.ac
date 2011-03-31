# Process this file with autoconf to produce a configure script.
#
#

# Prelude
AC_INIT(indexf,04.01,[ncl@astrax.fis.ucm.es])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([config.h.in])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs
AC_PROG_F77 # this is required to know which is $F77 (see Checks for libraries)
AC_PROG_CXX

# Safety check
AC_CONFIG_SRCDIR([src/indexf.cpp])

# Checks for libraries
PKG_CHECK_MODULES([PACKAGES], [cfitsio])
AC_CHECK_LIB([cpgplot], main,,AC_MSG_ERROR(cpgplot library not found))
# Note: -lgfortran (or -lg77) is required to link the executable file
AC_CHECK_LIB([$F77], main,,AC_MSG_ERROR($F77 library not found))
STTL_LIB_PGPLOT
LIBS="$LIBS $PGPLOT_LIBS"

# Definitions
DEFAULT_AUXDIR="/usr/local/share/indexf"
AUXDIR=${AUXDIR="$DEFAULT_AUXDIR"}
AC_ARG_VAR(AUXDIR,[Directory to install auxiliary xpgp files])

# Output files
AC_CONFIG_FILES([Makefile
                 src/installdir.h
                 src/Makefile
                 m4/Makefile
                 auxdir/Makefile])
AC_OUTPUT
