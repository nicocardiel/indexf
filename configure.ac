# Process this file with autoconf to produce a configure script.
#
# $Id$
#

# Prelude
AC_INIT(indexf,04.01,[ncl@astrax.fis.ucm.es])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([config.h.in])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs
AC_PROG_CXX

# Safety check
AC_CONFIG_SRCDIR([src/indexf.cpp])

# Checks for libraries
PKG_CHECK_MODULES([PACKAGES], [plplotd-c++ cfitsio])

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
