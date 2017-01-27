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

AX_APPEND_FLAG([-std=c99])

dnl Enable compiler flags
AX_CHECK_COMPILE_FLAG([-Werror], [WERROR=-Werror], [WERROR=])
AX_CHECK_COMPILE_FLAG([$WERROR -Wall],
  [AX_APPEND_FLAG([-Wall], [AM_CFLAGS])])
AX_CHECK_COMPILE_FLAG([$WERROR -Wextra],
  [AX_APPEND_FLAG([-Wextra], [AM_CFLAGS])])
AX_CHECK_COMPILE_FLAG([$WERROR -pedantic],
  [AX_APPEND_FLAG([-pedantic], [AM_CFLAGS])])

dnl Checks for libraries.
AC_SUBST([AM_CPPFLAGS], ['-I$(top_srcdir)/include'])

dnl Checks for header files.
AC_CHECK_HEADERS([errno.h, ftw.h, stdlib.h string.h sys/stat.h sys/types.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([mkdir, nftw, stat, strcspn])

dnl debugging information
AC_MSG_CHECKING(whether to enable debugging symbols)
AC_ARG_ENABLE([debug],
[AS_HELP_STRING([--enable-debug], [Turn on debugging])],
[
  AX_APPEND_FLAG([-O0])
  AX_APPEND_FLAG([-g])
  AC_MSG_RESULT(yes)
],
[
  AC_MSG_RESULT(no)
])

dnl debugging information
AC_MSG_CHECKING(whether to enable compiler warnings)
AC_ARG_ENABLE([warnings],
[AS_HELP_STRING([--enable-warnings], [Turn on warnings])],
[
  AX_APPEND_FLAG([-Wall])
  AX_APPEND_FLAG([-Wextra])
  AC_MSG_RESULT(yes)
],
[
  AC_MSG_RESULT(no)
])

AC_OUTPUT
