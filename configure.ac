AC_PREREQ(2.61)
AC_INIT(rmsgw, 2.5.1, info@nwdigitalradio.com)
AM_INIT_AUTOMAKE([-Wall -Werror dist-bzip2])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([pathnames.h.in])
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

# Check to see if there is an AX.25 subsystem installed by looking for axports
# Check and see if it is installed in /usr/local/etc/ax25, and set AXPORTS variable accordingly, 
# otherwiser we'll just default to /etc/ax25 where it normally would be found
AC_SUBST([AXPORTS])
AC_CHECK_FILE([/usr/local/etc/ax25/axports], [AXPORTS="/usr/local/etc/ax25/axports"], [AXPORTS="/etc/ax25/axports"])


# Checks for header files.
AC_HEADER_STDC
AC_INCLUDES_DEFAULT # Checks for sys/types.h, sys/stat.h, stdlib.h, string.h, memory.h, strings.h, inttypes.h, stdint.h and unistd.h
AC_CHECK_HEADERS([ctype.h endian.h errno.h fcntl.h math.h netdb.h netinet/in.h signal.h stdarg.h stdbool.h stdio.h sys/ipc.h sys/select.h sys/shm.h sys/socket.h sys/time.h sys/wait.h syslog.h time.h utime.h],,AC_MSG_ERROR([Required headers missing; aborting]))

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memmove memset socket strcasecmp strchr strcspn strdup strerror strrchr strspn strstr strtoul])

AC_CONFIG_FILES([Makefile \
	include/Makefile \
	lib/Makefile \
	rmsgw/Makefile \
	rmsgw_aci/Makefile \
	rmsgwmon/Makefile \
	rmschanstat/Makefile \
	devutils/Makefile \
	man/Makefile \
	contrib/Makefile \
	doc/Makefile \
	doc/assets/Makefile \
	etc/Makefile \
	etc/hooks/Makefile \
	admin/Makefile])

AC_OUTPUT
