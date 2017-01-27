dnl                                               -*- Autoconf -*-
dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([ladle], [0.0.0], [staticfox@staticfox.net])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([include/utils.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign nostdinc subdir-objects])

dnl Checks for programs.
AC_PROG_CC_C99

dnl Enable compiler flags
AX_CHECK_COMPILE_FLAG([-Werror], [WERROR=-Werror], [WERROR=])
AX_CHECK_COMPILE_FLAG([$WERROR -std=c99],
  [AX_APPEND_FLAG([-std=c99], [AM_CFLAGS])])
AX_CHECK_COMPILE_FLAG([$WERROR -Wall],
  [AX_APPEND_FLAG([-Wall], [AM_CFLAGS])])
AX_CHECK_COMPILE_FLAG([$WERROR -Wextra],
  [AX_APPEND_FLAG([-Wextra], [AM_CFLAGS])])
AX_CHECK_COMPILE_FLAG([$WERROR -pedantic],
  [AX_APPEND_FLAG([-pedantic], [AM_CFLAGS])])

dnl Checks for libraries.
AC_SUBST([AM_CPPFLAGS], ['-I$(top_srcdir)/include'])

dnl Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h sys/stat.h sys/types.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([mkdir, stat, strcspn])

AC_OUTPUT
