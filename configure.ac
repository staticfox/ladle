#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ladle], [0.0.0], [staticfox@staticfox.net])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([include/utils.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign nostdinc subdir-objects])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_SUBST([AM_CPPFLAGS], ['-I$(top_srcdir)/include'])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strcspn])

AC_OUTPUT
