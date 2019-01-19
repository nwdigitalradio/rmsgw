AC_PREREQ(2.61)
AC_INIT(rmsgw, 2.5.0, info@nwdigitalradio.com)
AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR

# Checks for programs.
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_PROG_RANLIB
AC_PROG_INSTALL

# Check for libxml2
PKG_CHECK_MODULES(XML, libxml-2.0 >= 2.4)

# Checks for libraries.
AC_SUBST(NCURSES_LIB)
AC_CHECK_LIB(ncurses, initscr, NCURSES_LIB="-lncurses", AC_MSG_ERROR(Could not find the ncurses library; aborting))



# Checks for header files.
AC_HEADER_STDC
AC_INCLUDES_DEFAULT # Checks for sys/types.h, sys/stat.h, stdlib.h, string.h, memory.h, strings.h, inttypes.h, stdint.h and unistd.h


AC_CONFIG_FILES([Makefile lib/Makefile rmsgw/Makefile rmsgw_aci/Makefile rmsgwmon/Makefile])

AC_OUTPUT
