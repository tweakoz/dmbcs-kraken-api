#    dmbcs-kraken-api   A C++ encapsulation of the API to Krakenʼs e-currency
#                       exchange
#  
#    Copyright (C) 2018  DM Bespoke Computer Solutions Ltd
#  
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or (at
#    your option) any later version.
#  
#    This program is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    General Public License for more details.
#  
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.


# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dmbcs-kraken-api], [1.0], [https://rdmp.org/dmbcs/contact])
AM_INIT_AUTOMAKE([silent-rules subdir-objects])
AM_SILENT_RULES([yes])
LT_INIT
AC_CONFIG_SRCDIR([src/dmbcs-kraken-api.cc])
AC_CONFIG_HEADERS([src/auto-config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.
PKG_CHECK_MODULES([third_party], [curlpp openssl])
AC_CHECK_LIB([m], [sin])

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday])

AC_CONFIG_FILES([src/makefile
                 makefile
                 dmbcs-kraken-api.pc])
AC_OUTPUT
